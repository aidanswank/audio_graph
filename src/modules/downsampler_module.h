#pragma once
#include "xmodule.h"

struct downsampler_module : xmodule {
    
    const int sample_rate = 44100;
    const int sample_size = 256;
    float rate_adjustment = 0.5; // adjust sample rate to half of original
    int output_sample_rate;

    downsampler_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_downsampler__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_downsampler__get_name();

