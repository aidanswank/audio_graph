#include "float_module.h"

float_module::float_module(audio_graph<xmodule*>& graph) : xmodule(graph)
{
    data = 0;
    name = module_float__get_name();
    config(1,1);
}

void float_module::process() {
//    xmodule::output_void_ptr = &data;
    float* output_audio_left = xmodule::output_audio[0].data();
    float* output_audio_right = xmodule::output_audio[1].data();
    for(int i = 0; i < 256; i++) {
        output_audio_left[i] = data;
        output_audio_right[i] = data;
    }
}

void float_module::show() {
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[0] );
//    ImGui::Text("in");
    ImNodes::EndInputAttribute();
    
    ImGui::PushItemWidth(100.0f);
//    ImGui::SliderFloat("##number_float", &data, 0.0f, 2000.0f);
    ImGui::InputFloat("##float_input", &data);
    ImGui::PopItemWidth();
    
    ImNodes::BeginOutputAttribute( output_attrs[0] );
//    ImGui::Text("out");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
}

xmodule* module_float__create(audio_graph<xmodule*>& graph)
{
    return new float_module(graph);
};

std::string module_float__get_name()
{
    return "float32";
};
