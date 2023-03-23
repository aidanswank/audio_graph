#pragma once
#include "xmodule.h"

struct multiply_module : xmodule {
    
//    std::vector<float>& input_signal_a;
//    std::vector<float>& input_signal_b;

    multiply_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    
};

xmodule* module_multiply__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_multiply__get_name();

