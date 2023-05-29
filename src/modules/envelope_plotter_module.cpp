//#pragma once;
#include "xmodule.h"
#include "envelope_plotter_module.h"

envelope_plotter_module::envelope_plotter_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_envelope_plotter__get_name();
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    
    duration_seconds = 1.0;
    trigger_flag = false;
    is_counting = false;
    current_sample = 0;
    current_amp = -1.0;
    curve_amount = 2.0;
    current_button_idx = -1;
    ImVec2 old_pos = {0,0};
    widget_hovered = false;
};

float envelope_plotter_module::get_envelope()
{
    if (points.empty() || duration_seconds == 0.0f)
        return 0.0f;

    float t = static_cast<float>(current_sample) / static_cast<float>(44100.0 * duration_seconds);
    float x = t * (points.size() - 1);
    int index = static_cast<int>(x);
    float frac = x - index;
    
//    print("frac",frac);

    if (index >= points.size() - 1)
        return points.back().y;

    float y0 = points[index].y;
    float y1 = points[index + 1].y;

    return lerp(y0, y1, 1.0f - pow(1.0f - frac, curve_amount));
}

void envelope_plotter_module::process()
{
    zero_audio(xmodule::output_audio, 256);
    
    float* output_audio_left = xmodule::output_audio[0].data();
    float* output_audio_right = xmodule::output_audio[1].data();
    
    if(trigger_flag)
    {
        print("audio trig");
        trigger_flag=false;
        is_counting=true;
    }
    
    for (int i = 0; i < 256; ++i) {
                
        if(is_counting)
        {
            float value = get_envelope();
            current_amp = (value/100.0)*-2 + 1;
//            std::cout << "Sample: " << current_sample << ", Envelope Value: " << sample << std::endl;
            current_sample++;
            if(current_sample >= int(duration_seconds*44100.0))
            {
    //          print("end");
                reset();
            }
        }
        
        output_audio_left[i] = current_amp;
        output_audio_right[i] = current_amp;
     }
};

void envelope_plotter_module::reset()
{
    is_counting=false;
    current_sample=0;
}

// passing a bunch of stuff by refrence cuz i might reuse this someday so i didnt want it to be tied to this class structure
void plotter_widget(std::vector<ImVec2> &points, int &current_button_idx, bool &widget_hovered, ImVec2 &old_pos, float& curve_amount)
{
    ImVec2 cursor_pos = ImGui::GetCursorPos();
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImVec2 relative_win_pos = ImGui::GetWindowPos();
    relative_win_pos.x -= ImGui::GetScrollX();
    relative_win_pos.y -= ImGui::GetScrollY();
    
    ImVec2 relative = relative_win_pos+cursor_pos;
    
    ImVec2 plotter_size = ImVec2(200,100);
    
    ImVec2 rect_min = relative; // Adjust the values here to position the rectangle
    ImVec2 rect_max = relative+plotter_size; // Adjust the values here to position the rectangle

    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0); //RGBA color values ranging from 0 to 1

    draw_list->AddRect(rect_min, rect_max, ImGui::GetColorU32(color));
    
    ImVec2 mouse_pos = ImGui::GetMousePos();
    
    ImVec2 relative_mouse_pos = relative - mouse_pos;
    
    bool shift_down = ImGui::GetIO().KeyShift;
    
    if(!shift_down)
    {
        current_button_idx = -1;
    }

    if (widget_hovered && !shift_down)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            float xabs = abs(relative_mouse_pos.x);
            float yabs = abs(relative_mouse_pos.y);

            // Find the appropriate position to insert the new point
            auto insert_iter = std::upper_bound(points.begin(), points.end(), ImVec2(xabs, yabs),
                [](const ImVec2& point1, const ImVec2& point2) {
                    return point1.x < point2.x;
                });

            // Insert the new point at the found position
            points.insert(insert_iter, ImVec2(xabs, yabs));
        }
    }

    for(int i = 0; i < points.size(); i++)
    {
        ImVec2 point_screen_pos = points[i] + relative;
        
        draw_list->AddCircle(point_screen_pos, 5, ImGui::GetColorU32(color), 20, 1);
        
        ImGui::SetItemAllowOverlap();
        ImGui::SetCursorPos(cursor_pos+points[i]-ImVec2(10,10));
        
        ImGui::PushID(i);
        if(shift_down)
        {
            ImGui::Button("",ImVec2(20,20));
            
            if(ImGui::IsItemClicked())
            {
                current_button_idx = i;
//                print("clicked button", current_button_idx);
                old_pos = points[i];
            }
            
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                print("delete", i);
                points.erase(points.begin() + i);
                current_button_idx = -1;
            }
        }
        ImGui::PopID();
        
//        print(current_button_idx);
        if(current_button_idx!=-1)
        {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                ImVec2 new_pos = ImGui::GetMouseDragDelta();
                //                    new_pos = new_pos - relative;
//                print(ImGui::GetMouseDragDelta().x, " ", ImGui::GetMouseDragDelta().y);
                ImVec2 pos = old_pos + new_pos;
                if(pos.x>=plotter_size.x)
                {
                    pos.x = plotter_size.x;
                }
                if(pos.x<=0)
                {
                    pos.x = 0;
                }
                if(pos.y>=plotter_size.y)
                {
                    pos.y = plotter_size.y;
                }
                if(pos.y<=0)
                {
                    pos.y = 0;
                }
                points[current_button_idx] = pos;
            }
        }
        
//        print("x",points[i].x/200,"y",points[i].y/100);
        
//        if(points.size()>=2 && (i != points.size()-1))
//        {
//            //            print("draw");
//
//            draw_list->AddLine(((points[i]+relative)), ((points[i+1]+relative)), ImGui::GetColorU32(color));
//        }
        
        if (points.size() >= 2)
        {
            for (int i = 0; i < points.size() - 1; i++)
            {
                ImVec2 p0 = points[i] + relative;
                ImVec2 p1 = points[i + 1] + relative;
                
                for (float x = p0.x; x < p1.x; x++)
                {
                    float t = (x - p0.x) / (p1.x - p0.x);
                    
//                    float envelopeValue = get_envelope2(x, points);
                    
                    float y = p0.y + (p1.y - p0.y) * (1.0f - pow(1.0f - t, curve_amount));
                    
                    ImVec2 pixel = ImVec2(x, y);
                    draw_list->AddCircleFilled(pixel, 1.0f, ImGui::GetColorU32(color));
                }
            }
        }
        
//        if (points.size() >= 2)
//        {
//            for (int i = 0; i < points.size() - 1; i++)
//            {
//                ImVec2 p0 = points[i] + relative;
//                ImVec2 p1 = p0 + ImVec2(50, 0);  // Control point 1
//                ImVec2 p2 = points[i + 1] + relative - ImVec2(50, 0);  // Control point 2
//                ImVec2 p3 = points[i + 1] + relative;
//
//                int numSegments = 20;  // Number of line segments to approximate the curve
//
//                for (int j = 0; j <= numSegments; j++)
//                {
//                    float t = static_cast<float>(j) / static_cast<float>(numSegments);
//                    float u = 1.0f - t;
//
//                    float b0 = u * u * u;
//                    float b1 = 3.0f * u * u * t;
//                    float b2 = 3.0f * u * t * t;
//                    float b3 = t * t * t;
//
//                    float x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
//                    float y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;
//                    ImVec2 pixel = ImVec2(x, y);
//                    draw_list->AddRectFilled(pixel, pixel + ImVec2(1, 1), ImGui::GetColorU32(color));
//
//                }
//            }
//        }
     
        

        
        //        if(!(points.size() % 2))
        //        {
        //            print("is even");
        //        }
    }
    
    ImGui::SetItemAllowOverlap();
    ImGui::SetCursorPos(cursor_pos);
    ImGui::Dummy(plotter_size);
    
    if(ImGui::IsItemHovered())
    {
//        print("hovered");
        widget_hovered=true;
        draw_list->AddCircle(mouse_pos, 5, ImGui::GetColorU32(color), 20, 1);
    } else {
        widget_hovered=false;
    }
}

void envelope_plotter_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    if(ImGui::Button("test"))
    {
//        print("trigggg");
        reset();
        trigger_flag=true;
    }
    
    ImGui::PushItemWidth(100);
    ImGui::SliderFloat("duration", &duration_seconds, 0, 2);
    ImGui::SliderFloat("curve", &curve_amount, 0.1, 10);
    ImGui::PopItemWidth();
    
    
    plotter_widget(points, current_button_idx, widget_hovered, old_pos, curve_amount);
        
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();

};

void envelope_plotter_module::save_state(nlohmann::json& object)
{
    std::vector<float> points_x;
    std::vector<float> points_y;
    for(int i = 0; i < points.size(); i++)
    {
        points_x.push_back(points[i].x);
        points_y.push_back(points[i].y);
    }
    object["points_x"] = points_x;
    object["points_y"] = points_y;
    object["curve_amount"] = curve_amount;
    object["duration_seconds"] = duration_seconds;
};

void envelope_plotter_module::load_state(nlohmann::json& object)
{
    std::vector<float> points_x = object["points_x"];
    std::vector<float> points_y = object["points_y"];
    
    curve_amount = object["curve_amount"];
    duration_seconds = object["duration_seconds"];
    
    for(int i = 0; i < points_x.size(); i++)
    {
        points.push_back(ImVec2(points_x[i],points_y[i]));
    }
};

xmodule* module_envelope_plotter__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new envelope_plotter_module(graph, click_pos);
};

std::string module_envelope_plotter__get_name()
{
    return "envelope plotter";
};


