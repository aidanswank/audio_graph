//#pragma once;
#include "xmodule.h"
#include "map_sig_module.h"

//https://gist.github.com/companje/29408948f1e8be54dd5733a74ca49bb9
float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

map_sig_module::map_sig_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_map_sig__get_name();
//    print("name!!",name);
    min_a=-1.0;
    max_a=1.0;
    min_b=1.0;
    max_b=-1.0;
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
};

void map_sig_module::process()
{
    zero_audio(xmodule::output_audio, 256);
    
    if(input_ids[0]) {
        xmodule* module_input = (xmodule*)graph.xmodules[ input_ids[0] ];

        for(int i = 0; i < 256; i++)
        {
            float input = module_input->output_audio[0][i];
            float res = map(input, min_a, max_a, min_b, max_b);
            output_audio[0][i] = res;
            output_audio[1][i] = res;
        }
    }
};

void map_sig_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    ImGui::PushItemWidth(100.0f);
    ImGui::InputFloat("min a", &min_a);
    ImGui::InputFloat("max a", &max_a);
    
    ImGui::InputFloat("min b", &min_b);
    ImGui::InputFloat("max b", &max_b);

//    ImGui::SliderFloat("##number_float", &data, min, max);
//    ImGui::InputFloat("##float_input", &data);
    ImGui::PopItemWidth();
    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};

void map_sig_module::save_state(nlohmann::json& object)
{
    object["min_a"] = min_a;
    object["min_b"] = min_b;
    object["max_a"] = max_b;
    object["max_b"] = max_a;

};

void map_sig_module::load_state(nlohmann::json& object)
{
    
};

xmodule* module_map_sig__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new map_sig_module(graph, click_pos);
};

std::string module_map_sig__get_name()
{
    return "map sig";
};


