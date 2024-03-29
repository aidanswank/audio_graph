//#pragma once;
#include "xmodule.h"
#include "polysampler.h"
#include <imfilebrowser.h>

struct polysampler_module : xmodule {
    
    polysampler* sampler;
    int current_waveform = 0;
    ImGui::FileBrowser *open_file_dialog;
    
    polysampler_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_polysampler__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_polysampler__get_name();

