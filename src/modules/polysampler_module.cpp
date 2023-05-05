//#pragma once;
#include "xmodule.h"
#include "polysampler_module.h"

polysampler_module::polysampler_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_polysampler__get_name();
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    
    synth = new polysampler(16);
};

void polysampler_module::process()
{
    zero_audio(xmodule::output_audio,256);
    float *audio_output_left_ptr = xmodule::output_audio[0].data();
    float *audio_output_right_ptr = xmodule::output_audio[1].data();
    
    if(input_ids[0][0]!=-1)
    {
        xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[0][0] ];
        
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
void polysampler_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};
void polysampler_module::save_state(nlohmann::json& object)
{
    
};
void polysampler_module::load_state(nlohmann::json& object)
{
    
};

xmodule* module_polysampler__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new polysampler_module(graph, click_pos);
};

std::string module_polysampler__get_name()
{
    return "polysynth";
};


