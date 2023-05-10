#pragma once
#include "xmodule.h"

#include "global_transport.h"
extern global_transport g_transport;

#include "interpolation.h"

//const char* event_parse_modes_items[] = { "FREQUENCY", "NORMALIZED" };
enum event_parse_modes { frequency, automation };

struct parse_module : xmodule {
    
    bool note_held;
    midi_note_message event2;
    int current_mode = 0;
    
    parse_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_parse__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_parse__get_name();

