//#pragma once;
#include "xmodule.h"
#include "parse_module.h"

parse_module::parse_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_parse__get_name();
//    print("name!!",name);
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    note_held = false;
    
//    new
};

void parse_module::process()
{
    int len = 256;
        
//    static midi_note_message event2;
    
    xmodule::output_audio[0].clear();
    xmodule::output_audio[1].clear();

    if(input_ids[0])
    {
        xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[0] ];

        for(int i = 0; i < midi_in_module->input_notes.size(); i++)
        {
            midi_note_message event = midi_in_module->input_notes[i];

            
            if(event.is_note_on)
            {
                note_held = true;
                event2 = event;
            } else {
                note_held = false;
            }
        }
        
        //while note held create an "audio signal" representing "automation" to be passed to other params
        
        if(note_held)
        {
            for(int j = 0; j < len; j++)
            {
                
                float t=(float)(g_transport.midi_tick_count-(event2.tick-(g_transport.ticks_per_sample*j) ))/(float)event2.duration;
                //                print("t from parse",t,j);
                
                float freq_a = midi2freq(event2.note_num+(event2.pitch_bend_a));
                float freq_b = midi2freq(event2.note_num+(event2.pitch_bend_b));
                float lerp_freq = lerp(freq_a,freq_b,pow(t,2.0));
                
                float lerp_note_nums = lerp(event2.note_num+(event2.pitch_bend_a),event2.note_num+(event2.pitch_bend_b),pow(t,2.0));
//                print("lerp_note_nums",lerp_note_nums/127.0);
                
                float cosine_interp_note_nums = CosineInterpolate(event2.note_num+(event2.pitch_bend_a),event2.note_num+(event2.pitch_bend_b), t);
                
                float sig = (lerp_note_nums / 127) * 2.0 - 1.0;
                
                if(current_mode==event_parse_modes::frequency)
                {
                    xmodule::output_audio[0].push_back(lerp_freq);
                    xmodule::output_audio[1].push_back(lerp_freq);
                }
                
                if(current_mode==event_parse_modes::automation)
                {
                    xmodule::output_audio[0].push_back(sig);
                    xmodule::output_audio[1].push_back(sig);
                }
                
                
//                if(output_ids[1][0]!=-1)
//                {
//                    print("???????afsdfasdf");
//                }
                
//                xmodule::input_void_ptr = (void*)lerp_note_nums;
                
//                xmodule::output_audio[2].push_back(lerp_note_nums);
//                xmodule::output_audio[3].push_back(lerp_note_nums);
                
//                float freq_a = midi2freq(event2.note_num+(event2.pitch_bend_a));
//                float freq_b = midi2freq(event2.note_num+(event2.pitch_bend_b));
//                float lerp_freq = lerp(freq_a,freq_b,pow(t,2.0));
//
//                print("lerp freq", lerp_freq, j);
                
            }
        }
    }
};

void parse_module::show()
    {
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("in");
    ImNodes::EndInputAttribute();
    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
//    ImGui::Text("freq");
    ImNodes::EndOutputAttribute();
    
    ImGui::PushItemWidth(100.0f);

    const char* event_parse_modes_names[] = { "FREQUENCY", "AUTOMATION" };
    if(ImGui::Combo("mode", &current_mode, event_parse_modes_names,
                 IM_ARRAYSIZE(event_parse_modes_names)))
    {
        print("yo!",current_mode);
    }
    
    ImGui::PopItemWidth();
    
    ImNodes::EndNode();
};

void parse_module::save_state(nlohmann::json& object)
{
    object["current_mode"] = current_mode;
};

void parse_module::load_state(nlohmann::json& object)
{
    current_mode = object["current_mode"];
};

xmodule* module_parse__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new parse_module(graph, click_pos);
};

std::string module_parse__get_name()
{
    return "event parse";
};


