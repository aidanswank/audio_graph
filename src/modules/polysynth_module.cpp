#pragma once
#include "xmodule.h"
#include "polysynth_module.h"

polysynth_module::polysynth_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_polysynth__get_name();
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    
    synth = new polysynth(16);
};

void polysynth_module::process()
{
    zero_audio(xmodule::output_audio,256);
    float *audio_output_left_ptr = xmodule::output_audio[0].data();
    float *audio_output_right_ptr = xmodule::output_audio[1].data();
    
    if(input_ids[0])
    {
        xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[0] ];
        
        for(int i = 0; i < midi_in_module->input_notes.size(); i++)
        {
            midi_note_message note = midi_in_module->input_notes[i];
//            print("incoming notes..", note.note_num, note.velocity, note.is_note_on, "+bend", note.pitch_bend_a, note.pitch_bend_b);
            synth->send_message(note);
        }
        
        synth->generate_samples(audio_output_left_ptr, 256);
        memcpy(audio_output_right_ptr, audio_output_left_ptr, 256*sizeof(float));
    }
};
void polysynth_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    
    const char* waveform_names[] = {
        "SINE", "COSINE", "TRIANGLE", "SQUARE", "RECTANGLE", "SAWTOOTH", "RAMP",
        "MODIFIED_TRIANGLE", "MODIFIED_SQUARE", "HALF_WAVE_RECTIFIED_SINE",
        "FULL_WAVE_RECTIFIED_SINE", "TRIANGULAR_PULSE", "TRAPEZOID_FIXED",
        "TRAPEZOID_VARIABLE"
    };

//    static int current_waveform = 0;
    ImGui::PushItemWidth(100.0f);
    
    if(ImGui::Combo("Waveform", &current_waveform, waveform_names,
                 IM_ARRAYSIZE(waveform_names)))
    {
        print("yo!",current_waveform);
        for(int i = 0; i < synth->voices.size(); i++)
        {
            synth->voices[i].synth->setWaveform((PolyBLEP::Waveform(current_waveform)));            
        }
    }

    ImGui::PopItemWidth();
//    Waveform selected_waveform = static_cast<Waveform>(current_waveform);
    
    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};
void polysynth_module::save_state(nlohmann::json& object)
{
    object["current_waveform"] = current_waveform;
//    print(object);
    
};
void polysynth_module::load_state(nlohmann::json& object)
{
    check_and_load(object, "current_waveform", &current_waveform);
    current_waveform = object["current_waveform"];
    print("polysynth",object,current_waveform);
    synth->set_waveform(current_waveform);
//    check_and_load(object, "resonance", &resonance);
};

xmodule* module_polysynth__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new polysynth_module(graph, click_pos);
};

std::string module_polysynth__get_name()
{
    return "polysynth";
};


