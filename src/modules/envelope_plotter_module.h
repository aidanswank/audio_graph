#pragma once
#include "imgui_utils.h"
#include "envelope_plotter_module.h"
#include "interpolation.h"

struct line_segment
{
    ImVec2 p1;
    ImVec2 p2;
};

struct envelope_plotter_module : xmodule {
    
    float duration_seconds;
    bool trigger_flag;
    bool is_counting;
    int current_sample;
    std::vector<ImVec2> points;
    float previous_pos;
    
    float y_a;
    float y_b;
    
    float current_amp;
    
    envelope_plotter_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;
    
    float get_envelope();

};

xmodule* module_envelope_plotter__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_envelope_plotter__get_name();

