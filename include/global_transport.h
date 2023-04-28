#pragma once

#include "MidiFile.h"

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
    smf::MidiFile* midifile; // not sure gonna keep this
};

float get_ms_per_tick(int bpm, int tpq);

float samples_to_ticks(int current_sample, float ms_per_tick, int sample_rate);
