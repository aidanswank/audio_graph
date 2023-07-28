#pragma once
#include "xmodule.h"

struct tapedelay_module : xmodule {
    
    int loop_length_samples = 6000;
    float buffer[100000] = {0};
    float buffer_copy[100000] = {0};
    float counter = 0.0;
    float tapespeed;
    float feedback;
    
    tapedelay_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_tapedelay__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_tapedelay__get_name();

