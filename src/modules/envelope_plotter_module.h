#pragma once
#include "imgui_utils.h"
#include "envelope_plotter_module.h"
#include "interpolation.h"

struct envelope_plotter_module : xmodule {
    
    float duration_seconds;
    bool trigger_flag;
    bool is_counting;
    int current_sample;
    std::vector<ImVec2> points;
    float current_amp;
    float curve_amount;
    bool widget_hovered;
    int current_button_idx = -1;
    ImVec2 old_pos = {0,0};
    
    envelope_plotter_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;
    
    // envelope specific
    float get_envelope();
    void reset();

};

xmodule* module_envelope_plotter__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_envelope_plotter__get_name();

