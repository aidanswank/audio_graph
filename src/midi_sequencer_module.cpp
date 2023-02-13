#include "midi_sequencer_module.h"


midi_sequencer::midi_sequencer(audio_graph<xmodule*>& graph) : xmodule(graph)
{
    name = module_midi_sequencer__get_name();
    config(0,1);
    
//    MidiNoteMessage fakenote;
//    fakenote.noteNum=65;
//    fakenote.isNoteOn=true;
//    fakenote.velocity=1;
//    input_notes.push_back(fakenote);
//
}

void midi_sequencer::show()
{
    ImNodes::BeginNode(xmodule::id);
   
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginOutputAttribute( output_attrs[0] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
}

void midi_sequencer::process()
{
    input_notes.clear();
//    int tstart = samples_to_ticks(g_transport.sample_count, g_transport.ms_per_tick, 44100);
    float block_size_in_ticks = samples_to_ticks(256, g_transport.ms_per_tick, 44100);
//    print("tick start",g_transport.midi_tick_count,"tick block",block_size_in_ticks);
    
    smf::MidiEventList midi_track = g_transport.midifile[0][0];

    for(int i = 0; i < midi_track.size(); i++)
    {
//        print(g_transport.midifile[0][i].tick);
        smf::MidiEvent *event = &midi_track[i];
        if(event->tick==g_transport.midi_tick_count||event->tick==g_transport.midi_tick_count+(int)block_size_in_ticks)
        {
//            print("note",event->getKeyNumber());
            MidiNoteMessage note;
            note.velocity = event->getVelocity();
            note.noteNum = event->getKeyNumber();
            note.isNoteOn = event->isNoteOn();

            input_notes.push_back(note);
        }
    }
}

xmodule* module_midi_sequencer__create(audio_graph<xmodule*>& graph)
{
    return new midi_sequencer(graph);
}

std::string module_midi_sequencer__get_name()
{
    return "midi sequencer";
}
