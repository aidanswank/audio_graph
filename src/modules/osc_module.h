//#pragma once;
#include "xmodule.h"
#include "PolyBLEP.h"
#include "global_transport.h"
extern global_transport g_transport;

struct osc_module : xmodule {
    
    PolyBLEP *blep;
    float freq = 440.0f;
    bool isLFO = false;
    const char* current_item = "SINE";
    
    osc_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_osc__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_osc__get_name();
