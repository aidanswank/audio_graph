#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "vendor/PolyBLEP.h"
#include "midi.h"
#include "global_transport.h"
//#include "fVoice.hpp"
//#include "json.hpp"
//using nlohmann::json;

struct voice
{
    bool is_active;
    int note_num;
    PolyBLEP *synth;
    float slope;
    voice(int sample_rate)
    {
        synth = new PolyBLEP(sample_rate);
        is_active = false;
        note_num = -1;
        slope = 0;
    }
};

// for load single cycle waveforms or other samples
class polysampler
{
public:
//    unsigned int voice_count = 16;
//    std::vector<PolyBLEP> voices;
//    std::vector<bool> voices_active;
//    std::vector<int> voices_note_num;
    std::vector<voice> voices;

    float *mixed_stream;

    int buffer_size;
    int sample_rate;

    polysampler(int voice_count);
    void send_message(midi_note_message midi_message);
    int get_active_voices();
    int find_free_voice();
    void generate_samples(float *stream, int len);
};
