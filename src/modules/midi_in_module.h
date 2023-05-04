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

double midi2freq(int n);

struct midi_in_module : xmodule {
    
    RtMidiIn* midiin_ptr;
    //    std::vector<MidiNoteMessage> notes;
    moodycamel::ConcurrentQueue<midi_note_message> notesQueue;
    std::vector<std::string> port_names;
    
    int current_midi_note = 0;
    
    midi_in_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;

    void process() override;
    void show() override {
        ImNodes::BeginNode(xmodule::id);
        //        ImGui::Dummy(ImVec2(80.0f, 45.0f));
        //        print(modules[ input_ids[0] ]->id);
        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();
        
        static const char* current_item = NULL;
        
        if(port_names.size()>0)
        {
            ImGui::PushItemWidth(200);
            if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
            {
                for (int n = 0; n < port_names.size(); n++)
                {
                    bool is_selected = (current_item == port_names[n].c_str()); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(port_names[n].c_str(), is_selected)){
                        current_item = port_names[n].c_str();
                        //                        print("fresshhh");
                        midiin_ptr->closePort();
                        midiin_ptr->openPort(n);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
        }
        
        //        ImNodes::BeginInputAttribute( xmodule::id );
        //        ImGui::Text("input");
        //        ImNodes::EndInputAttribute();
        
        ImNodes::BeginOutputAttribute( output_attrs[0] );
        ImGui::Text("output");
        ImNodes::EndOutputAttribute();
        
        ImNodes::EndNode();
    };
    
};

xmodule* module_midi_in__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_midi_in__get_name();
