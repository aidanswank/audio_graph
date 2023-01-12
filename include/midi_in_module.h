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

struct midi_in_module : xmodule {
    
    RtMidiIn* midiin_ptr;
//    std::vector<MidiNoteMessage> notes;
    moodycamel::ConcurrentQueue<MidiNoteMessage> notesQueue;
    std::vector<std::string> port_names;
    
    midi_in_module(audio_graph<xmodule*>& graph);
    void process() override;
    void show() override {
        ImNodes::BeginNode(xmodule::id);
    //        ImGui::Dummy(ImVec2(80.0f, 45.0f));
    //        print(modules[ input_ids[0] ]->id);
        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();

//        ImNodes::BeginInputAttribute( xmodule::id );
//        ImGui::Text("input");
//        ImNodes::EndInputAttribute();

        ImNodes::BeginOutputAttribute( output_attrs[0] );
        ImGui::Text("output");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    };
    void poll() override {};
};

xmodule* module_midi_in__create(audio_graph<xmodule*>& graph);
std::string module_midi_in__get_name();
