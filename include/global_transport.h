#pragma once

#include "MidiFile.h"

#include "midi.h"
#include <map>

struct global_transport
{
    bool is_playing=false;
    float tempo = 120.0f; // default tempo
    float ticks_per_quarter_note = 96; // dont hardcode but ableton exports at this resolution so its fine for now
    float midi_tick_count = 0;
    float current_seconds = 0;
    float ms_per_tick = 0;
    int sample_count = 0;
    int bar = 0;
    int current_pattern = 0;
    int current_pattern_open = 0;
    int current_column_open = 0;

//    smf::MidiFile midi_file; // not sure gonna keep this
    std::map<int, smf::MidiFile> midi_module_map;
    std::vector<smf::MidiFile> midi_patterns; // all patterns
    std::map<int, std::vector<int>> pattern_map; // map of node_id->vector of pattern indices

    smf::MidiFile *midifile; // not sure gonna keep this
    
    //ahhhhh
    float ticks_per_sample = 0;
};

float get_ms_per_tick(int bpm, int tpq);

float samples_to_ticks(int current_sample, float ms_per_tick, int sample_rate);
