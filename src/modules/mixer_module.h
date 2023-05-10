#pragma once
#include "xmodule.h"

struct mixer_module : xmodule {
    
//    float slider_0;
//    float slider_1;
//    float slider_2;
//    float slider_3;
//    float slider_4;
//    float slider_5;
    std::vector<float> slider_vals;
    std::vector<STEREO_AUDIO> all_input_audio;
    int num_mixer_inputs;

    mixer_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_mixer__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_mixer__get_name();

