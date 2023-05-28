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
    y_a = 0.0;
    y_b = 0.0;
};

float envelope_plotter_module::get_envelope()
{
    if (points.empty() || duration_seconds == 0.0f)
        return 0.0f;

    float t = static_cast<float>(current_sample) / static_cast<float>(44100.0 * duration_seconds);
    float x = t * (points.size() - 1);
    int index = static_cast<int>(x);
    float frac = x - index;

    if (index >= points.size() - 1)
        return points.back().y;

    float y0 = points[index].y;
    float y1 = points[index + 1].y;
    return lerp(y0, y1, frac);
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
        
        float sample = -1;
        
        if(is_counting)
        {
            float value = get_envelope();
            sample = (value/100.0)*-2 + 1;
//            std::cout << "Sample: " << current_sample << ", Envelope Value: " << sample << std::endl;
            
            current_sample++;
            if(current_sample >= int(duration_seconds*44100.0))
            {
                print("end");
                is_counting=false;
                current_sample=0;
            }
        }
        
        output_audio_left[i] = sample;
        output_audio_right[i] = sample;
     }
};

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
        trigger_flag=true;
    }
    
    ImGui::PushItemWidth(100);
    ImGui::SliderFloat("duration", &duration_seconds, 0, 2);
    ImGui::PopItemWidth();
    
    static bool widget_hovered = false;
        
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

    static int current_button_idx = -1;
    static ImVec2 old_pos = {0,0};
    
    if(!shift_down)
    {
        current_button_idx = -1;
    }
    
    if(widget_hovered && !shift_down)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            //        print("yo");
            float xabs = abs(relative_mouse_pos.x);
            float yabs = abs(relative_mouse_pos.y);
            print(xabs, yabs);
            points.push_back(ImVec2(xabs,yabs));
        }
    }
    
    for(int i = 0; i < points.size(); i++)
    {
        ImVec2 point_screen_pos = points[i] + relative;
        
        draw_list->AddCircle(point_screen_pos, 5, ImGui::GetColorU32(color), 20, 1);
        
//        ImGui::SetCursorPos(point_screen_pos);
//        ImGui::SameLine();
        ImGui::SetItemAllowOverlap();
        ImGui::SetCursorPos(cursor_pos+points[i]-ImVec2(10,10));
        ImGui::PushID(i);
        if(shift_down)
        {
            if(ImGui::Button("",ImVec2(20,20)))
            {
                current_button_idx = i;
                print("clicked button", current_button_idx);
                old_pos = points[i];
            }
//            ImGui::Button("",ImVec2(20,20));
//            if(ImGui::IsItemHovered())
//            {
////                print("hovering button", i);
//                current_button_idx = i;
//                old_pos = points[i];
//            }
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
        
        if(points.size()>=2 && (i != points.size()-1))
        {
            //            print("draw");
            
            draw_list->AddLine(((points[i]+relative)), ((points[i+1]+relative)), ImGui::GetColorU32(color));
        }
        
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
    
//    if(ImGui::IsItemActivated())
//    {
//        print("focused");
//    }
        
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();

};

void envelope_plotter_module::save_state(nlohmann::json& object)
{
    
};

void envelope_plotter_module::load_state(nlohmann::json& object)
{
    
};

xmodule* module_envelope_plotter__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new envelope_plotter_module(graph, click_pos);
};

std::string module_envelope_plotter__get_name()
{
    return "envelope plotter";
};


