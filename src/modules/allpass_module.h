#pragma once
#include "xmodule.h"
#include "allpass_filter.h"

struct allpass_module : xmodule {
    
    AllpassFilter *allpass;
    
    std::vector<AllpassFilter*> allpass_bank;
    
    allpass_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void process_sample(float input);
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_allpass__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_allpass__get_name();

