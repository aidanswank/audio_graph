//#pragma once;
#include "xmodule.h"
#include "map_sig_module.h"

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


