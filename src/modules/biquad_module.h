#pragma once
#include "xmodule.h"
#include "vendor/Biquad.h"

struct biquad_module : xmodule {
    
    Biquad *filter;
    float cutoff;
    float resonance;
    int current_mode;
    
    biquad_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_biquad__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_biquad__get_name();

