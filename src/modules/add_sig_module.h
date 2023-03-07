#pragma once
#include "xmodule.h"

struct add_module : xmodule {

    add_module(audio_graph<xmodule*>& graph);
    void process() override;
    void show() override;
    
};

xmodule* module_add__create(audio_graph<xmodule*>& graph);
std::string module_add__get_name();

