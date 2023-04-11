#include "multiply_sig_module.h"

void multiply_module::show() {
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

void multiply_module::process() {
    
    float *audio_output_left = xmodule::output_audio[0].data();
    float *audio_output_right = xmodule::output_audio[1].data();
    xmodule* module_input_a = NULL;
    xmodule* module_input_b = NULL;

    float input_a_audio[256] = {0};
    float input_b_audio[256] = {0};
    float final_audio[256] = {0};
        
    if(input_ids[0][0]!=-1) {  //check if module is connected
        module_input_a = (xmodule*)graph.xmodules[ input_ids[0][0] ];
//        
//        if(module_input_a->output_void_ptr) { //check is
//            float single_number = *(float*)module_input_a->output_void_ptr;
////            print("void ptr input",*single_number);
//            for(int i = 0; i < 256; i++)
//            {
//                input_a_audio[i] = single_number;
//            }
//        } else {
//            memcpy(input_a_audio, module_input_a->output_audio[0].data(), 256*sizeof(float));
//        }
        memcpy(input_a_audio, module_input_a->output_audio[0].data(), 256*sizeof(float));
    }
    
    if(input_ids[1][0]!=-1) {
        module_input_b = (xmodule*)graph.xmodules[ input_ids[1][0] ];
        memcpy(input_b_audio, module_input_b->output_audio[0].data(), 256*sizeof(float));
    }
    
    for(int i = 0; i < 256; i++) {
        final_audio[i] = input_a_audio[i] * input_b_audio[i];
    }
    
    for(int i = 0; i < 256; i++) {
        audio_output_left[i] = final_audio[i];
        audio_output_right[i] = final_audio[i];
    }
    
}

multiply_module::multiply_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos) {
    config(2,1);
    name = module_multiply__get_name(); // this feels weird
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
}

xmodule* module_multiply__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new multiply_module(graph, click_pos);
};

std::string module_multiply__get_name()
{
    return "multiply signal";
};

void multiply_module::save_state(nlohmann::json &object)
{
    
};

void multiply_module::load_state(nlohmann::json &object)
{
    
};
