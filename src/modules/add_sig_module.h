#pragma once
#include "xmodule.h"

struct add_module : xmodule {

    add_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    
};

xmodule* module_add__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_add__get_name();

