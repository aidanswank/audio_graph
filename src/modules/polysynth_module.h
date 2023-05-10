//#pragma once;
#include "xmodule.h"
#include "polysynth.h"

struct polysynth_module : xmodule {
    
    polysynth* synth;
    int current_waveform = 0;
    
    polysynth_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_polysampler__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_polysampler__get_name();

