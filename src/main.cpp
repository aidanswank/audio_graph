#include <iostream>
#include <vector>
#include "SDL2/SDL.h"

#include "global_transport.h"
global_transport g_transport;
// modules
#include "xmodule.h"
#include "midi_in_module.h"
#include "vst3_module.h"
#include "cjfilter_module.h"
#include "audio_output_module.h"
#include "osc_module.h"
#include "midi_sequencer_module.h"
#include "float_module.h"
#include "multiply_sig_module.h"
#include "add_sig_module.h"
#include "polysynth.h"
#include "polysynth_module.h"
#include "parse_module.h"
#include "map_sig_module.h"
#include "tube_module.h"
#include "mixer_module.h"
#include "polysampler.h"
#include "polysampler_module.h"
#include "downsampler_module.h"
#include "envelope_plotter_module.h"

//#include "graph.h"
#include "audio_interface.h"
//#include "audio_callback.h"
#include "MidiFile.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
json current_patch;


#include "piano_roll.h"
static bool isOpenSequencerWindow;
//smf::MidiFile mymidifile;

//// Global variables
//int tick_counter = 0;  // Counter for MIDI clock ticks
//double tick_interval;  // Time interval between MIDI clock ticks in seconds
//double bpm = 120;   // Default BPM
//
//// Set BPM function
//void set_bpm(double new_bpm)
//{
//    bpm = new_bpm;
//    tick_interval = (60.0 / bpm) / 24.0;  // Update tick interval based on new BPM
//}

static int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{

    audio_interface* interface = (audio_interface*)userData;
    audio_graph<xmodule*>* graph = (audio_graph<xmodule*>*)interface->data;

    if(g_transport.is_playing)
    {
    if(graph->xmodules.size()>0 && graph->root_id!=-1)
    {
        int root_node = graph->root_id;
        
        graph->clear();

        graph->DFS(root_node);
        
        float *output = (float*)outputBuffer;
        
        for( uint i=0; i<framesPerBuffer; ++i )
        {
            //        float w = (float)(rand()%100)/1000.0f; //testing with white noise
            //        output[i * 2] = w;
            //        output[i * 2 + 1] = w;  /* right */
            output[i * 2]       = graph->xmodules[root_node]->output_audio[0][i]; /* left */
            output[i * 2 + 1]   = graph->xmodules[root_node]->output_audio[1][i];  /* right */
        }
    }
    
    // have to increment sample count after module processing or we skip first note
        
        g_transport.ms_per_tick = get_ms_per_tick(g_transport.tempo,g_transport.ticks_per_quarter_note);
        float midi_tick_inc = samples_to_ticks(framesPerBuffer, g_transport.ms_per_tick, 44100);
        g_transport.ticks_per_sample = samples_to_ticks(1, g_transport.ms_per_tick, 44100);
        g_transport.midi_tick_count += midi_tick_inc;
        //        print("tick counter",g_transport.midi_tick_count);
        g_transport.sample_count += framesPerBuffer;
        g_transport.current_seconds = g_transport.sample_count/44100.0f;
        
        
        if(g_transport.midi_tick_count>=96*4)
        {
            g_transport.midi_tick_count=0;
            g_transport.current_seconds=0;
            g_transport.current_pattern++;
            if(g_transport.current_pattern>=4)
            {
                g_transport.current_pattern=0;
            }
//            print(g_transport.current_pattern);
        }
        
    }

    return 0;
}

#include "im_wrap.h"

void audio_settings_gui(audio_interface* interface)
{
    static bool audio_settings_open = true;
    if(audio_settings_open)
    {
        
        ImGui::Begin("audio settings", &audio_settings_open);
        
        static const char* current_item = NULL;
        
        if (ImGui::BeginCombo("input devices", current_item)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < interface->device_infos.size(); n++)
            {
                if(interface->device_infos[n].maxInputChannels>0)
                {
                    bool is_selected = (current_item == interface->device_infos[n].name); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(interface->device_infos[n].name, is_selected))
                    {
                        current_item = interface->device_infos[n].name;
                        interface->set_param(true, n);
                        
                        // dont set mic and then retry callback
                        // if i select speaker then mic with this uncommented i get glitchy audioß
                        //                    interface->try_params();
                        //                    interface->turn_on(audio_callback);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
            }
            ImGui::EndCombo();
        }
        
        static const char* current_item2 = NULL;
        if (ImGui::BeginCombo("outout devices", current_item2))
        {
            for (int n = 0; n < interface->device_infos.size(); n++)
            {
                if(interface->device_infos[n].maxOutputChannels>0)
                {
                    bool is_selected = (current_item2 == interface->device_infos[n].name);
                    if (ImGui::Selectable(interface->device_infos[n].name, is_selected))
                    {
                        current_item2 = interface->device_infos[n].name;
                        print("dev id", n);
                        interface->set_param(false, n);
                        interface->try_params();
                        interface->turn_on(audio_callback);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::End();
    }
}

void link_module(int start_attr, int end_attr, audio_graph<xmodule*>* graph)
{
        
    print("start", start_attr, "end", end_attr);
    print("arr2id map start", graph->attr2id[start_attr], "end", graph->attr2id[end_attr]);

    std::vector<int>& start_attr_vector  = graph->xmodules[ graph->attr2id[start_attr] ]->output_ids[ graph->attr2outslot[start_attr] ];

    std::vector<int>& end_attr_vector = graph->xmodules[ graph->attr2id[end_attr] ]->input_ids[ graph->attr2inslot[end_attr] ];

    // if there was nothing connected before (-1) put it in the first slot which is 0
    // else if theres already a cable connected push it onto the vector (e.g the final audio output were multiple cables to be connected to same slot)

    if(start_attr_vector[0]==-1)
    {
        start_attr_vector[0] = graph->attr2id[end_attr];
    } else {
        start_attr_vector.push_back(graph->attr2id[end_attr]);
    }

    if(end_attr_vector[0]==-1)
    {
        end_attr_vector[0] = graph->attr2id[start_attr];
    } else {
        end_attr_vector.push_back( graph->attr2id[start_attr] );
    }

    graph->links.push_back(std::make_pair(start_attr, end_attr));
}

void remove_link(int edge_id, audio_graph<xmodule*>* graph)
{
    
    int start_attr = graph->links[ edge_id ].first;
    int end_attr = graph->links[ edge_id ].second;
//                        print("start_attr", start_attr, "end attr",end_attr);
//                        print(
//                              "start id",graph->xmodules[ graph->attr2id[start_attr] ]->id,
//                              "end id",graph->xmodules[ graph->attr2id[end_attr] ]->id
//                        );
        
    // why did i write end_attr twice in attr2outslot and attr2inslot was that intentional or glitch??? i forgot how this works
//    std::vector<int>& start_attr_vector  = graph->xmodules[ graph->attr2id[start_attr] ]->output_ids[ graph->attr2outslot[end_attr] ];

    std::vector<int>& start_attr_vector  = graph->xmodules[ graph->attr2id[start_attr] ]->output_ids[ graph->attr2outslot[start_attr] ];
    std::vector<int>& end_attr_vector    = graph->xmodules[ graph->attr2id[end_attr] ]->input_ids[ graph->attr2inslot[end_attr] ];
  
//
//    start_attr_vector[ graph->attr2inslot[start] ] = -1;
//    end_attr_vector[ graph->attr2inslot[end_attr] ] = -1;

    
//    print("!!!!!!!!!!!",graph->attr2inslot[start_attr],graph->attr2outslot[end_attr]);
    
    // fill input and output slots with -1
    for(int i = 0; i < start_attr_vector.size(); i++)
    {
        start_attr_vector[i] = -1;
    }

    for(int i = 0; i < end_attr_vector.size(); i++)
    {
        end_attr_vector[i] = -1;
    }
    
//    int start = graph->links[edge_id].first;
//    int end = graph->links[edge_id].second;
//    int index = 0;
//    for (auto& element : current_patch["links"])
//    {
////        link_module(element["start"], element["end"], graph);
//        if(element["start"]==start&&element["end"]==end)
//        {
//            print("found link in patch");
//            current_patch["links"].erase(current_patch["links"].begin())
//        }
//        index++;
//    }
    
    current_patch["links"].erase(current_patch["links"].begin() + edge_id);
    print("erased!, current",current_patch);
        
    // BREAKS WHEN DELETETING MULTIPLE NODES GOTTA FIX
    graph->links.erase(graph->links.begin() + edge_id);

}


#include <imfilebrowser.h>
class user_interface
{
public:
    im_wrap ui;
    audio_graph<xmodule*>* graph;
//    std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&)>* factory_map;
    std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&, ImVec2)>* factory_map;

    audio_interface *my_audio_interface;
    ImGui::FileBrowser *open_file_dialog;
    ImGui::FileBrowser *save_file_dialog;
    
    user_interface(SDL_Window* window,
                   SDL_GLContext gl_context,
                   audio_graph<xmodule*>* p_graph,
                   std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&, ImVec2)>* p_factory_map,
                   audio_interface *p_audio_interface)
    {
        factory_map = p_factory_map;
        graph = p_graph;
        my_audio_interface = p_audio_interface;
        print("ui init");
        ui.init(window,gl_context);
        isOpenSequencerWindow=true;
        
        ImNodesStyle& style = ImNodes::GetStyle();
            style.Flags |= ImNodesStyleFlags_GridLinesPrimary | ImNodesStyleFlags_GridSnapping;
        // (optional) set browser properties
        
        open_file_dialog = new ImGui::FileBrowser();
        open_file_dialog->SetTitle("open file");
        open_file_dialog->SetTypeFilters({ ".json" });
        
        save_file_dialog = new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
        save_file_dialog->SetTitle("save file");
        save_file_dialog->SetTypeFilters({ ".json" });

    }
    ~user_interface()
    {
        print("ui shutdown");
        ui.shutdown();
    }
    void load_patch(std::string file_path)
    {
        std::ifstream i(file_path);
//        json current_patch;
        i >> current_patch;
//        print(current_patch);
        
//        current_patch = loaded_patch;
        
//        xmodule* m = factory_map->at(it->first)(*graph, click_pos);
        
        graph->root_id = current_patch["root_id"];
        
//        g_transport.tempo = current_patch["bpm"];
    
        for (auto& element : current_patch["nodes"])
        {
            float x = element["x"];
            float y = element["y"];
            std::string name = element["name"].get<std::string>();
            ImVec2 pos(x,y);
            
            xmodule* created_module = factory_map->at(name)(*graph, pos);
            created_module->load_state(element);
            graph->xmodules.push_back( created_module );

            print("name",name,"x",x,"y",y);
        }
        
        for (auto& element : current_patch["links"])
        {
            print("link!!!!!",element);
            int start = element["start"];
            int end = element["end"];
//            print("start",start,"end",end);
            link_module(start, end, graph);
        }
        
//        print("tracks!!",current_patch["tracks"]);
        json j =current_patch["tracks"];
        for (auto& [key, value] : j.items())
        {
//          std::cout << key << " : " << value << "\n";
            if(value[0].is_null())
            {
                print("pattern 0 null ignore");
            } else {
                int node_id = std::stoi( key );
                print("node id from track key str", node_id);
                
                
                // THIS IS BAD!!
                // since i changed event classes in lib i have to modify memory after i "add" it to add pitch bend stuff
                // TODO REMOVE MIDI FILE LIBRARY AND MOVE EVERYTHING TO OWN CLASS OR SOMETHING
                // I SPEND TOO MUCH TIME CONVERTING BETWEEN THESE
                smf::MidiFile fake;
                g_transport.midi_module_map[node_id] = fake;

                for(int i = 0; i < value[0].size(); i++)
                {
//                    print(value[0][i]);
                    json jevent=value[0][i]["event"];
//                    midi_track.append(MidiEvent &event)
                    print("jevent",jevent);
                    int note_num = jevent["note_num"];
                    int pitch_bend_a = jevent["pitch_bend_a"];
                    int pitch_bend_b = jevent["pitch_bend_b"];
//                    print("yuhhhhhh",note_num,pitch_bend_a,pitch_bend_b);
                    smf::MidiEvent event;
//                    event.key = jevent["note_num"];
                    if(jevent["is_note_on"])
                    {
                        event.makeNoteOn(0, note_num, 60);
                    } else {
                        event.makeNoteOff(0, note_num, 60);
                    }
                    event.tick = jevent["tick"];
                    event.track = 0;
                    event.pitch_bend_a = pitch_bend_a;
                    event.pitch_bend_b = pitch_bend_a;
                    g_transport.midi_module_map[node_id].addEvent(event);
                    g_transport.midi_module_map[node_id].linkNotePairs();
                    //modify memory afterwards for custom things
                    g_transport.midi_module_map[node_id][0][i].pitch_bend_a = pitch_bend_a;
                    g_transport.midi_module_map[node_id][0][i].pitch_bend_b = pitch_bend_b;
                    

                }
                
            }
        }
        
    }
    
    void save_patch(std::string filepath)
    {

        current_patch["filepath"] = filepath;
        
        current_patch["bpm"] = g_transport.tempo;

//        print( current_patch["name"] );

        // get current position of the modules and update json patch state
        for(int i = 0; i < graph->xmodules.size(); i++)
        {
//                            print(graph->xmodules[i]->id);
            int id = graph->xmodules[i]->id;
            ImVec2 pos = ImNodes::GetNodeScreenSpacePos(id);
            print("pos",pos.x,pos.y);
            current_patch["nodes"][id]["x"] = pos.x;
            current_patch["nodes"][id]["y"] = pos.y;
            
            graph->xmodules[i]->save_state(current_patch["nodes"][id]);
            
//            current_patch["tracks"] = tracks;

        }
        
        json tracks;
        for ( auto it = g_transport.midi_module_map.begin(); it != g_transport.midi_module_map.end(); ++it  )
        {
            smf::MidiFile& midi_file = it->second;
            smf::MidiEventList& midi_events = midi_file[0];
            json events;
            for(int i = 0; i < midi_events.size(); i++)
            {
                midi_note_message note;
                smf::MidiEvent &event = midi_events[i];
                note.velocity = event.getVelocity();
                note.note_num = event.getKeyNumber();
                note.is_note_on = event.isNoteOn();
                note.pitch_bend_a = event.pitch_bend_a;
                note.pitch_bend_b = event.pitch_bend_b;
                note.duration = event.getTickDuration();
                note.tick = event.tick;
                
                json myevent;
                myevent["event"]["note_num"] = note.note_num;
                myevent["event"]["is_note_on"] = note.is_note_on;
                myevent["event"]["velocity"] = note.velocity;
                myevent["event"]["pitch_bend_a"] = note.pitch_bend_a;
                myevent["event"]["pitch_bend_b"] = note.pitch_bend_b;
                myevent["event"]["tick"] = note.tick;
                events.push_back(myevent);
            }
            std::string key = std::to_string(it->first);
            tracks[key].push_back(events);
        }
        print(tracks);
        current_patch["tracks"] = tracks;

        // write prettified JSON to another file
        std::ofstream o(filepath);
        o << std::setw(4) << current_patch << std::endl;
        print("saved patch!", current_patch);
    }
    
    void update()
    {
        
        ImGuiStyle& style = ImGui::GetStyle();
        style.ItemSpacing = ImVec2(4,4);
        style.FrameBorderSize = 1;
        
        ui.new_frame();

        audio_settings_gui(my_audio_interface);
        

//        ui.update();
        
        ImGui::ShowDemoWindow();
        
//        piano_roll_window(&isOpenSequencerWindow, g_transport.midi_file);
//        piano_roll_window2(&isOpenSequencerWindow, g_transport.midi_track);
        sequencer_controls_window();
        pattern_editor_window(&isOpenSequencerWindow, graph);
        piano_roll_window(&isOpenSequencerWindow, g_transport.midi_module_map[g_transport.current_pattern_open]);
        clip_timeline_window(&isOpenSequencerWindow);
        
        static bool node_editor_active = true;
        if(node_editor_active)
        {
            ImGui::Begin("patch editor", &node_editor_active, ImGuiWindowFlags_MenuBar);
            
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open..", "Ctrl+O"))
                    {
                        open_file_dialog->Open();
                    }
                    if (ImGui::MenuItem("Save", "Ctrl+S"))
                    {
                        save_file_dialog->Open();
                    }
    //                if (ImGui::MenuItem("Close", "Ctrl+W"))  { my_tool_active = false; }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            
            ImNodes::BeginNodeEditor();
            
            const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
            ImNodes::IsEditorHovered() && ImGui::IsMouseReleased(1);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
            if (!ImGui::IsAnyItemHovered() && open_popup)
            {
                ImGui::OpenPopup("add node");
            }

            if (ImGui::BeginPopup("add node"))
            {
                const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
                
//                 loop through factory map to display names and push back if selected
                for (std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&, ImVec2)>::iterator it = factory_map->begin(); it != factory_map->end(); ++it) {
//                    std::cout << it->first << std::endl;
                    if (ImGui::MenuItem(it->first.c_str()))
                    {
                        print(click_pos.x, click_pos.y, it->first.c_str());
                        xmodule* m = factory_map->at(it->first)(*graph, click_pos);
                        
                        if(m->name=="audio output") // special case set root node
                        {
                            graph->root_id = m->id;
                        }
                        
                        graph->xmodules.push_back( m );
                        
                        print("name??",m->name);
                        
                        json mod_storage;
                        mod_storage["name"] = m->name;
                        mod_storage["x"] = click_pos.x;
                        mod_storage["y"] = click_pos.y;
                        current_patch["nodes"].push_back(mod_storage);
                        current_patch["root_id"] = graph->root_id;
//                        print(current_patch);
                    }
                }
                
                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            for(uint i = 0; i < graph->xmodules.size(); ++i)
            {
                graph->xmodules[i]->show();
            }
//
            for (int i = 0; i < graph->links.size(); ++i)
            {
                  const std::pair<int, int> p = graph->links[i];
                  // in this case, we just use the array index of the link
                  // as the unique identifier
                  ImNodes::Link(i, p.first, p.second);
            }

            ImNodes::MiniMap();
            ImNodes::EndNodeEditor();

            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                link_module(start_attr, end_attr, graph);
                json link;
                link["start"] = start_attr;
                link["end"] = end_attr;
                current_patch["links"].push_back(link);
                print(current_patch);
            }
  
            
//            {
//                int link_id;
//                if (ImNodes::IsLinkDestroyed(&link_id))
//                {
//                    print("link id",link_id);
////                    graph2.erase_edge(link_id);
//                }
//            }
            
            // audio nodes deleting links
            {
                const int num_selected = ImNodes::NumSelectedLinks();
                if (num_selected > 0 && ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_X))
                {
                    static std::vector<int> selected_links;
                    selected_links.resize(static_cast<size_t>(num_selected));
                    ImNodes::GetSelectedLinks(selected_links.data());
                    for (const int edge_id : selected_links)
                    {
                        print("ei",edge_id);
//
                        remove_link(edge_id, graph);

                    }
                }
            }
            
//            // Note that since many nodes can be selected at once, we first need to query the number of
//            // selected nodes before getting them.
//            const int num_selected_nodes = ImNodes::NumSelectedNodes();
//            if (num_selected_nodes > 0)
//            {
//              std::vector<int> selected_nodes;
//              selected_nodes.resize(num_selected_nodes);
//              ImNodes::GetSelectedNodes(selected_nodes.data());
//            }

            ImGui::End();
        }

        open_file_dialog->Display();
        save_file_dialog->Display();

        if(open_file_dialog->HasSelected())
        {
            std::cout << "Selected filename" << open_file_dialog->GetSelected().string() << std::endl;
            load_patch(open_file_dialog->GetSelected().string());
            open_file_dialog->ClearSelected();
        }
        
        if(save_file_dialog->HasSelected())
        {
            std::cout << "Selected filename" << save_file_dialog->GetSelected().string() << std::endl;
//            load_patch(open_file_dialog->GetSelected().string());
            save_patch(save_file_dialog->GetSelected().string());
            save_file_dialog->ClearSelected();
        }

        // Create a window called "My First Tool", with a menu bar.
//        static bool debug_active = true;
//        if(debug_active)
//        {
//            ImGui::Begin("debug", &debug_active);
//            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
//                           1000.0f / ImGui::GetIO().Framerate,
//                           ImGui::GetIO().Framerate);
//            
////            ImDrawList *draw_list = ImGui::GetWindowDrawList();
//            
//            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
//            ImVec2 vMax = ImGui::GetWindowContentRegionMax();
//
//            vMin.x += ImGui::GetWindowPos().x;
//            vMin.y += ImGui::GetWindowPos().y;
//            vMax.x += ImGui::GetWindowPos().x;
//            vMax.y += ImGui::GetWindowPos().y;
//
//            ImGui::GetForegroundDrawList()->AddRect( vMin, vMax, IM_COL32( 255, 255, 0, 255 ) );
//            
//            ImGui::End();
//        }

        ui.render();
    }
};

#include <map>
#include <functional>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // to make macos happy
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // uint32_t WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    uint32_t window_flags = SDL_WINDOW_OPENGL | SDL_RENDERER_PRESENTVSYNC | SDL_WINDOW_RESIZABLE;
    SDL_Window *window = SDL_CreateWindow("editor", 0, 0, 320, 240, window_flags);
    // assert(Window);
    if (window == NULL)
    {
        printf("failed to create window!");
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    
    SDL_Event event;
    
    // graph that contains xmodule nodes
    audio_graph<xmodule*> graph;
    // idk just easier right now for vst
    graph.event = &event;
    
//    std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&)> module_factory_map;
    std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&, ImVec2)> module_factory_map;

    module_factory_map[module_audio_output__get_name()]    = &module_audio_output__create;
    module_factory_map[module_midi_in__get_name()]         = &module_midi_in__create;
    module_factory_map[module_vst3_instrument__get_name()] = &module_vst3_instrument__create;
    module_factory_map[module_cjfilter__get_name()]        = &module_cjfilter__create;
    module_factory_map[module_osc__get_name()]             = &module_osc__create;
    module_factory_map[module_midi_sequencer__get_name()]  = &module_midi_sequencer__create;
    module_factory_map[module_float__get_name()]           = &module_float__create;
    module_factory_map[module_multiply__get_name()]        = &module_multiply__create;
    module_factory_map[module_add__get_name()]             = &module_add__create;
    module_factory_map[module_polysynth__get_name()]       = &module_polysynth__create;
    module_factory_map[module_parse__get_name()]           = &module_parse__create;
    module_factory_map[module_map_sig__get_name()]         = &module_map_sig__create;
    module_factory_map[module_tube__get_name()]            = &module_tube__create;
    module_factory_map[module_mixer__get_name()]           = &module_mixer__create;
    module_factory_map[module_polysampler__get_name()]     = &module_polysampler__create;
    module_factory_map[module_downsampler__get_name()]     = &module_downsampler__create;
    module_factory_map[module_envelope_plotter__get_name()]= &module_envelope_plotter__create;

    // set up audio interface and open stream
    audio_interface interface;
    interface.scan_devices();
//    interface.sample_rate=44100;
//    interface.buffer_size=256;
    interface.pass_userdata(&graph);

////    smf::MidiFile mymidifile;
//    int midifile_err = mymidifile.read("/Users/aidan/dev/cpp/dfs_modules/pitch_bend.mid");
//    if (midifile_err == 0)
//    {
//        std::cout << "error loading midi!! :(" << std::endl;
//    }
//    g_transport.midifile=&mymidifile;
//    mymidifile.linkNotePairs();
    
//    ui.load_patch("/Users/aidan/dev/cpp/dfs_modules/build/Debug/vst_example.json");
//    ui.load_patch("/Users/aidan/dev/cpp/dfs_modules/build/Debug/mypatch.json");
//        ui.load_patch("/Users/aidan/dev/cpp/dfs_modules/build/Debug/simple_fm.json");

    // only run on start up for debug
    interface.init_devices(44100, 256, 2, 4);  //  sample rate, buffer size, input_device_id, output_device_id
//    interface.turn_on(audio_callback);
    
    user_interface ui(window, gl_context, &graph, &module_factory_map, &interface);
    
    ui.load_patch("1plotter_example.json");
    
    bool is_running = true;
//
    // main window loop
    while (is_running)
    {
        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
               print("quitttttt");
               is_running = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_GUI)
                {
                    if (SDL_HasClipboardText())
                    {
                        char* clipboardText = SDL_GetClipboardText();
                        if (clipboardText != nullptr)
                        {
                            std::cout << "Clipboard Text: " << clipboardText << std::endl;
                            SDL_free(clipboardText);
                        }
                    }
                }
            }
            for(uint i = 0; i < graph.xmodules.size(); ++i)
            {
                if(graph.xmodules[i]->name=="vst instrument") // sdl2 polling for easyvst
                {
                    vst3_midi_instrument* vst_mid_inst_ptr = (vst3_midi_instrument*)graph.xmodules[i];
                    vst_mid_inst_ptr->poll();
                }
            }
        }
                
        ui.update();
        
    }
    
//    gui.shutdown();
    SDL_Quit();

    return 0;
}
