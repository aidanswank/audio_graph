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
#include "interpolation.h"

struct synth_voice
{
//    bool is_active;
//    int note_num;
//    float pitch_bend_a;
//    float pitch_bend_b;
    midi_note_message note;
    PolyBLEP *synth;
    float freq=-1;
//    float slope;
    synth_voice(int sample_rate)
    {
        synth = new PolyBLEP(sample_rate);
        synth->setWaveform(PolyBLEP::SAWTOOTH);
        note.is_note_on = false;
        note.note_num = -1;
//        note.
//        is_active = false;
//        note_num = -1;
//        slope = 0;
    }
};

// for load single cycle waveforms or other samples
class polysynth
{
public:
//    unsigned int voice_count = 16;
//    std::vector<PolyBLEP> voices;
//    std::vector<bool> voices_active;
//    std::vector<int> voices_note_num;
    std::vector<synth_voice> voices;

    float *mixed_stream;

    int buffer_size;
    int sample_rate;

    polysynth(int voice_count);
    void set_waveform(int waveform)
    {
        for(int i = 0; i < voices.size(); i++)
        {
            voices[i].synth->setWaveform(PolyBLEP::Waveform(waveform));
        }
    }
    void send_message(midi_note_message midi_message);
    int get_active_voices();
    int find_free_voice();
    void generate_samples(float *stream, int len);
};
