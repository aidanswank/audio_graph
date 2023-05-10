#pragma once
#include "xmodule.h"

struct map_sig_module : xmodule {
    
    float min_a;
    float max_a;
    float min_b;
    float max_b;
    
    map_sig_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_map_sig__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_map_sig__get_name();

