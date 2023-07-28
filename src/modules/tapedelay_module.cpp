//#pragma once;
#include "xmodule.h"
#include "tapedelay_module.h"

tapedelay_module::tapedelay_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_tapedelay__get_name();
    tapespeed = 1.0;
    feedback = 0.0;
    
//    print("name!!",name);
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
};

void tapedelay_module::process()
{
    zero_audio(xmodule::output_audio, 256);
    
//    for(int i = 0; i < num_mixer_inputs; i++)
//    {
        if(input_ids[0]) {
            xmodule* module_input = (xmodule*)graph.xmodules[ input_ids[0] ];
//            print(i,"input connected");
            for(int i = 0; i < 256; i++)
            {
                buffer[(int)floor(counter)] = module_input->output_audio[0][i];
                
                buffer[(int)floor(counter)] = buffer[(int)floor(counter)] + (buffer_copy[(int)counter]*feedback);

                if(counter>=loop_length_samples-1)
                {
                    // smooth_loop(buffer, loop_length_samples-1);
                    // crossfade(buffer, loop_length_samples-1, 500);
                    memcpy(buffer_copy,buffer,(loop_length_samples-1)*sizeof(float));
                    counter=0;
                    print("end");
                } else {
                    counter += tapespeed;
                }
                
                output_audio[0][i] += buffer[(int)floor(counter)];
                output_audio[1][i] += buffer[(int)floor(counter)];
            }
        }
        
//    }
};

void tapedelay_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImGui::PushItemWidth(100.0f);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
//    char label[16];
//    sprintf(label, "delay", tapespeed);
    ImGui::SliderFloat("label", &tapespeed, 0.0f, 2.0f);
    ImGui::SliderFloat("feedback", &feedback, 0.0f, 0.98f);

    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImGui::PopItemWidth();
    
    ImNodes::EndNode();
};

void tapedelay_module::save_state(nlohmann::json& object)
{
    
};

void tapedelay_module::load_state(nlohmann::json& object)
{
    
};

xmodule* module_tapedelay__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new tapedelay_module(graph, click_pos);
};

std::string module_tapedelay__get_name()
{
    return "tapedelay";
};


