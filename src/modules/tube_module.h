#pragma once
#include "xmodule.h"

struct tube_module : xmodule {
    
    float gain;
    float shape1;
    float shape2;
    tube_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_tube__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_tube__get_name();

