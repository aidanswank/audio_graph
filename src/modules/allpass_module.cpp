//#pragma once;
#include "xmodule.h"
#include "allpass_module.h"

allpass_module::allpass_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_allpass__get_name();
    //    print("name!!",name);
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    
    allpass = new AllpassFilter(0.9, 0.1, 44100);
    
//    for(int i = 0; i < 64; i++)
//    {
//        AllpassFilter *ap = new AllpassFilter(maxDelay, initialDelay, gain);
//        allpass_bank.push_back(ap);
//    }
    
    
};

void allpass_module::process()
{
    float *audio_output_left = xmodule::output_audio[0].data();
    float *audio_output_right = xmodule::output_audio[1].data();
    xmodule* module_input = NULL;
    
    float input_audio[256] = {0};
//    float output_audio[256] = {0};
        
    if(input_ids[0]!=-1) {  //check if module is connected
        module_input = (xmodule*)graph.xmodules[ input_ids[0] ];
        memcpy(input_audio, module_input->output_audio[0].data(), 256*sizeof(float));
    }
    
    // Process the input signal through the allpass filter
    for (int i = 0; i < 256; i++) {
        float output = allpass->process(input_audio[i]);
//        std::cout << "Input: " << input_audio[i] << "\tOutput: " << output << std::endl;
        audio_output_left[i] = output;
        audio_output_right[i] = output;
    }
    
};

void allpass_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};

void allpass_module::save_state(nlohmann::json& object)
{
    
};

void allpass_module::load_state(nlohmann::json& object)
{
    
};

xmodule* module_allpass__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new allpass_module(graph, click_pos);
};

std::string module_allpass__get_name()
{
    return "allpass";
};


