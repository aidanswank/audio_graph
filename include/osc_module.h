//#pragma once;
#include "xmodule.h"
#include "PolyBLEP.h"

struct osc_module : xmodule {
    
    PolyBLEP *blep;
    float freq = 440.0f;
    bool isLFO = false;
    const char* current_item = NULL;
    
    osc_module(audio_graph<xmodule*>& graph);
    void process() override;
    void show() override;
    
};

xmodule* module_osc__create(audio_graph<xmodule*>& graph);
std::string module_osc__get_name();
