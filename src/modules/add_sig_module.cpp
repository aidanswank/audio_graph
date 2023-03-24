#include "add_sig_module.h"

void add_module::show() {
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[0] );
    ImGui::Text("A");
    ImNodes::EndInputAttribute();
    
    ImNodes::BeginInputAttribute( input_attrs[1] );
    ImGui::Text("B");
    ImNodes::EndInputAttribute();
    
    ImGui::PushItemWidth(100.0f);
    
    ImGui::PopItemWidth();
    
    ImNodes::BeginOutputAttribute( output_attrs[0] );
    ImGui::Text("out");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
}

void add_module::process() {
    
    float *audio_output_left = xmodule::output_audio[0].data();
    float *audio_output_right = xmodule::output_audio[1].data();
    xmodule* module_input_a = NULL;
    xmodule* module_input_b = NULL;

    float summed_audio[256] = {0};
        
    if(input_ids[0][0]!=-1) {
        module_input_a = (xmodule*)graph.xmodules[ input_ids[0][0] ];
    }
    
    if(input_ids[1][0]!=-1) {
        module_input_b = (xmodule*)graph.xmodules[ input_ids[1][0] ];
    }
    
    for(int i = 0; i < 256; i++) {
        if(module_input_a!=NULL)
        {
            summed_audio[i]+=module_input_a->output_audio[0][i];
        }
        
        if(module_input_b!=NULL)
        {
            summed_audio[i]+=module_input_b->output_audio[0][i];
        }
    }
    
    for(int i = 0; i < 256; i++) {
        audio_output_left[i] = summed_audio[i];
        audio_output_right[i] = summed_audio[i];
    }
}

add_module::add_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos) {
    config(2,1);
    name = module_add__get_name(); // this feels weird
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
}

xmodule* module_add__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new add_module(graph, click_pos);
};

std::string module_add__get_name()
{
    return "add signal";
};
