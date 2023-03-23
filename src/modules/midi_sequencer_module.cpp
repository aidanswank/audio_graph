#include "midi_sequencer_module.h"


midi_sequencer::midi_sequencer(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    name = module_midi_sequencer__get_name();
    config(0,1);
    ImNodes::SetNodeScreenSpacePos(id, click_pos);

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
    
    float tick_start = g_transport.midi_tick_count;
    float block_size_in_ticks = samples_to_ticks(256, g_transport.ms_per_tick, 44100);

    float tick_end = tick_start + block_size_in_ticks;
    
    smf::MidiEventList midi_events = g_transport.midifile[0][0];
    
    for(int i = 0; i < midi_events.size(); i++)
    {
        smf::MidiEvent event = midi_events[i];

        if(event.tick>=tick_start&&event.tick<=tick_end)
        {
            MidiNoteMessage note;
            note.velocity = event.getVelocity();
            note.noteNum = event.getKeyNumber();
            note.isNoteOn = event.isNoteOn();

            print("midi event",note.isNoteOn,note.noteNum,note.velocity);
            input_notes.push_back(note);
        }
    }
}

xmodule* module_midi_sequencer__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new midi_sequencer(graph, click_pos);
}

std::string module_midi_sequencer__get_name()
{
    return "midi sequencer";
}
