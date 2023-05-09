#include "midi_sequencer_module.h"


midi_sequencer::midi_sequencer(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    name = module_midi_sequencer__get_name();
    config(0,1);
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    
//    g_transport.midi_module_map[midi_sequencer::id];
    
//    g_transport.current_pattern++;
    
    for(int i = 0; i < 4; i++)
    {
        g_transport.pattern_map[midi_sequencer::id].push_back(0);
    }
    
    current_pattern_open = 0;
    
//    g_transport.pattern_map[midi_sequencer::id];
//    g_transport.midi_module_map[midi_sequencer::id];
    
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
    
//    if(ImGui::Button("open"))
//    {
//        is_piano_roll_open=!is_piano_roll_open;
//        print("yes?",is_piano_roll_open);
//    }
    
//    if(is_piano_roll_open)
//    {
////        g_transport.midi_module_map[midi_sequencer::id];
//        piano_roll_window(&is_piano_roll_open, g_transport.midi_module_map[midi_sequencer::current_pattern_open], midi_sequencer::id);
//    }
    
    
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
    
//    print(g_transport.midi_tick_count);

    float tick_end = tick_start + block_size_in_ticks;
    
//    std::cout << tick_start << " " << tick_end << std::endl;
    // if not refrence losses pitch bend for some reason
    
    // super long what the heck is going on
    smf::MidiFile& midi_file = g_transport.midi_module_map[g_transport.pattern_map[midi_sequencer::id][g_transport.current_pattern]];
    smf::MidiEventList& midi_events = midi_file[0];
    
    print("id",midi_sequencer::id,"cur pattern",g_transport.pattern_map[midi_sequencer::id][g_transport.current_pattern]);

    
    if(g_transport.is_playing)
    {
        for(int i = 0; i < midi_events.size(); i++)
        {
            smf::MidiEvent &event = midi_events[i];
            
            if(event.tick>=tick_start&&event.tick<=tick_end)
            {
                //uhhhhh
                midi_note_message note;
                note.velocity = event.getVelocity();
                note.note_num = event.getKeyNumber();
                note.is_note_on = event.isNoteOn();
                note.pitch_bend_a = event.pitch_bend_a;
                note.pitch_bend_b = event.pitch_bend_b;
                note.duration = event.getTickDuration();
                note.tick = event.tick;
                
//                print("bend a",event.pitch_bend_a,"b",event.pitch_bend_b);
//                print("midi event",note.is_note_on,note.note_num,note.velocity);
                if((note.note_num!=-1) && (note.velocity!=-1))
                {
                    input_notes.push_back(note);
                } else {
                    print("negative midi note?");
                    print(note.note_num,note.pitch_bend_a,note.pitch_bend_b);
                }
            }
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

void midi_sequencer::save_state(nlohmann::json &object)
{
//    print(g_transport.pattern_map[midi_sequencer::id].size());
    
    object["pattern"] = g_transport.pattern_map[midi_sequencer::id];
//    smf::MidiFile& midi_file = g_transport.midi_module_map[g_transport.pattern_map[midi_sequencer::id][g_transport.current_pattern]];
};

void midi_sequencer::load_state(nlohmann::json &object)
{
    
};
