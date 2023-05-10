#pragma once

#include <iostream>
#include "xmodule.h"
#include "piano_roll.h"
#include "global_transport.h"

extern global_transport g_transport;

struct midi_sequencer : xmodule {
    
//    smf::MidiEventList midi_events;
    bool is_piano_roll_open = true;
    int current_pattern_open = -1;
    
    float current_freq = 0;

    midi_sequencer(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

};

xmodule* module_midi_sequencer__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_midi_sequencer__get_name();
