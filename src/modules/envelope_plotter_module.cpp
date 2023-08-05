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
    current_amp = 0.0;
    curve_amount = 2.0;
    current_button_idx = -1;
    ImVec2 old_pos = {0,0};
    widget_hovered = false;
};

float envelope_plotter_module::get_envelope() {
    if (points.empty() || duration_seconds == 0.0f)
        return 0.0f;

    float t = (float)(current_sample) / (float)(44100.0 * duration_seconds);
    float x = t * (points.size() - 1);
    int index = (int)(x);
    float frac = x - index;
    
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
    
    
    if(input_ids[0])
    {
        xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[0] ];
        
        for(int i = 0; i < midi_in_module->input_notes.size(); i++)
        {
            midi_note_message event = midi_in_module->input_notes[i];
            
            
            if(event.is_note_on)
            {
//                note_held = true;
//                event2 = event;
//                print("on!!");
                reset();
                trigger_flag=true;
            } else {
//                note_held = false;
            }
        }
        
        //while note held create an "audio signal" representing "automation" to be passed to other params
    }
    
    for (int i = 0; i < 256; ++i) {
                
        if(is_counting)
        {
            float value = get_envelope();
            current_amp = (value/100.0)*-1.0 + 1.0;
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
    
    static std::vector<float> segment_curve_amount(16, 1);

    bool shift_down = ImGui::GetIO().KeyShift;
    bool alt_down = ImGui::IsKeyDown((ImGuiKey)SDL_SCANCODE_LALT);

    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly=true;
        
    if(!shift_down)
    {
        current_button_idx = -1;
    }

    if (widget_hovered && !shift_down && !alt_down)
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
    
    int highlighted_index = -1;
    
    // Iterate through the points to find the segment where the mouse is located
    if(points.size()>=2)
    {
        for (int i = 0; i < points.size() - 1; i++)
        {
            ImVec2 p0 = points[i] + relative;
            ImVec2 p1 = points[i + 1] + relative;
            
            if (mouse_pos.x >= p0.x && mouse_pos.x <= p1.x)
            {
                float segment_width = p1.x - p0.x;
                float mouse_offset = mouse_pos.x - p0.x;
                float t = mouse_offset / segment_width;
                
                // Check if the mouse is within a specific threshold
                if (t >= 0.1f && t <= 0.9f)
                {
                    highlighted_index = i;
//                    print("highlight idx", highlighted_index);
                    break;
                }
            }
        }
    }
    
    static ImVec2 alt_delta = {0,0};


    // Check if the mouse is inside the plotter rectangle
//    if (mouse_pos.x >= rect_min.x && mouse_pos.x <= rect_max.x &&
//        mouse_pos.y >= rect_min.y && mouse_pos.y <= rect_max.y)
//    {
        // Draw the highlight rectangle if a segment is highlighted
        if (highlighted_index != -1)
        {
            ImVec2 p0 = points[highlighted_index] + relative;
            ImVec2 p1 = points[highlighted_index + 1] + relative;
            ImVec2 rect_min = ImVec2(p0.x, relative.y);
            ImVec2 rect_max = ImVec2(p1.x, relative.y + plotter_size.y);
            ImVec4 highlight_color = ImVec4(1.0f, 0.0f, 0.0f, 0.5f);  // RGBA color values
            
            if(alt_down)
            {
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    alt_delta = ImGui::GetMouseDragDelta();
//                    float mod = (alt_delta.y / 100.0);
//                    print("alt delta", alt_delta.y);
                    float preview = alt_delta.y / 100;
//                    segment_curve_amount[ highlighted_index ] = preview;
                    
//                    segment_curve_amount[ highlighted_index ] = alt_delta.y / 100.0;
                }

                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                {
                    float mod = (alt_delta.y / 100.0);
                    print("use", mod);
                    segment_curve_amount[ highlighted_index ] += mod;
                    alt_delta = {0.0,0.0};
                    
                }
            }
            
//            segment_curve_amount[ highlighted_index ] = alt_delta.y;
//            print( "index", highlighted_index, segment_curve_amount[ highlighted_index] , "delta", alt_delta.y);

            draw_list->AddRectFilled(rect_min, rect_max, ImGui::GetColorU32(highlight_color));
//        }
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
                
                // Get the x-coordinate of the previous point
               float prev_x = (current_button_idx > 0) ? points[current_button_idx - 1].x : -INFINITY;
               
               // Get the x-coordinate of the next point
               float next_x = (current_button_idx < points.size() - 1) ? points[current_button_idx + 1].x : INFINITY;
               
               // Restrict x-coordinate based on adjacent points
               if (pos.x < prev_x)
                   pos.x = prev_x;
               else if (pos.x > next_x)
                   pos.x = next_x;
               
               // Restrict y-coordinate of the dragged point
               if (pos.y >= plotter_size.y)
                   pos.y = plotter_size.y;
               else if (pos.y <= 0)
                   pos.y = 0;
                       
                
                points[current_button_idx] = pos;
            }
        }
        
        if (points.size() >= 2)
        {
            const int num_samples = 32;  // Adjust the number of samples as needed

            for (int i = 0; i < points.size() - 1; i++)
            {
                ImVec2 p0 = points[i] + relative;
                ImVec2 p1 = points[i + 1] + relative;

                for (int j = 0; j < num_samples; j++)
                {
                    float t = static_cast<float>(j) / (num_samples - 1);
//                    float t_curve = 1.0f - pow(1.0f - t, segment_curve_amount[i] );
                    
                    float t_curve = 1.0f - pow(1.0f - t, curve_amount );

                    float x = p0.x + (p1.x - p0.x) * t;
                    float y = p0.y + (p1.y - p0.y) * t_curve;

                    ImVec2 pixel = ImVec2(x, y);

                    if (j > 0)
                    {
//                        ImVec2 prev_pixel = ImVec2(p0.x + (p1.x - p0.x) * (static_cast<float>(j - 1) / (num_samples - 1)), p0.y + (p1.y - p0.y) * (1.0f - pow(1.0f - (static_cast<float>(j - 1) / (num_samples - 1)), segment_curve_amount[i] )));
                        
                        ImVec2 prev_pixel = ImVec2(p0.x + (p1.x - p0.x) * (static_cast<float>(j - 1) / (num_samples - 1)), p0.y + (p1.y - p0.y) * (1.0f - pow(1.0f - (static_cast<float>(j - 1) / (num_samples - 1)), curve_amount )));

                        draw_list->AddLine(prev_pixel, pixel, ImGui::GetColorU32(color));
                    }

                    if (i == points.size() - 2 && j == num_samples - 1)
                    {
                        ImVec2 next_pixel = p1;
                        draw_list->AddLine(pixel, next_pixel, ImGui::GetColorU32(color));
                    }
                }
            }
        }
    }
    
    ImGui::SetItemAllowOverlap();
    ImGui::SetCursorPos(cursor_pos);
    ImGui::Dummy(plotter_size);
    ImGui::SetCursorPos(cursor_pos);
    ImGui::InvisibleButton("test",plotter_size);
    
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
    
    // make slider feel linear
    char val_str[64];
    sprintf(val_str, "%.3f", curve_amount);
    curve_amount = log(curve_amount);
    ImGui::SliderFloat("curve", &curve_amount, log(0.01), log(100.0), val_str);
    curve_amount = exp(curve_amount);
    
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


