#pragma once

#include <iostream>
#include "xmodule.h"
#include <vector>
#include <cstddef>
#include <rtmidi/RtMidi.h>
#include <concurrentqueue/concurrentqueue.h>

//struct MidiNoteMessage
//{
//    int noteNum = -1;
//    float velocity = 0.0f;
//    bool isNoteOn = false;
//};

struct rt_midi_in : xmodule {
    
    RtMidiIn* midiin_ptr;
//    std::vector<MidiNoteMessage> notes;
    moodycamel::ConcurrentQueue<MidiNoteMessage> notesQueue;
    std::vector<std::string> port_names;
    
    rt_midi_in(int id);
    void process(std::vector<xmodule*>& modules) override;
};
