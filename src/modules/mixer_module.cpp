//#pragma once;
#include "xmodule.h"
#include "mixer_module.h"

mixer_module::mixer_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    num_mixer_inputs = 6;
    config(num_mixer_inputs,1);
    name = module_mixer__get_name();
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    
    for(int i = 0; i < num_mixer_inputs; i++)
    {
//        zero_audio(all_input_audio[i], 256);
        STEREO_AUDIO audio;
        slider_vals.push_back(0.75);
        zero_audio(audio, 256);
        all_input_audio.push_back(audio);
    }
//    std::vector<float> channel_1(len, 0.0f);

};

void mixer_module::process()
{
    zero_audio(xmodule::output_audio, 256);
    
    for(int i = 0; i < num_mixer_inputs; i++)
    {
        if(input_ids[i][0]!=-1) {
            xmodule* module_input = (xmodule*)graph.xmodules[ input_ids[i][0] ];
//            print(i,"input connected");
            for(int j = 0; j < 256; j++)
            {
                output_audio[0][j] += module_input->output_audio[0][j] * pow(slider_vals[i],2);
                output_audio[1][j] += module_input->output_audio[1][j] * pow(slider_vals[i],2);
            }
        }
        
    }
};

void mixer_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImGui::PushItemWidth(100.0f);

    for(int i = 0; i < num_mixer_inputs; i++)
    {
        ImNodes::BeginInputAttribute( input_attrs[ i ] );
        char label[16];
        sprintf(label, "##slider_%d", i);
        ImGui::SliderFloat(label, &slider_vals[i], 0.0f, 1.0f);
        ImNodes::EndInputAttribute();
    }
    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImGui::PopItemWidth();
    
    ImNodes::EndNode();
};

void mixer_module::save_state(nlohmann::json& object)
{
    for(int i = 0; i < num_mixer_inputs; i++)
    {
        char label[16];
        sprintf(label, "##slider_%d", i);
        object[label]=slider_vals[i];
    }
};

void mixer_module::load_state(nlohmann::json& object)
{
    for(int i = 0; i < num_mixer_inputs; i++)
    {
        char label[16];
        sprintf(label, "##slider_%d", i);
        slider_vals[i] = object[label];
    }
};

xmodule* module_mixer__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new mixer_module(graph, click_pos);
};

std::string module_mixer__get_name()
{
    return "mini mixer";
};


