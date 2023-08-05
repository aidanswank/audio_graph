#include "im_wrap.h"
#include "xmodule.h"
#include "audio_interface.h"

extern json current_patch;

void audio_settings_gui(audio_interface* interface);

void link_module(int start_attr, int end_attr, audio_graph<xmodule*>* graph);

void remove_link(int edge_id, audio_graph<xmodule*>* graph);

#include "piano_roll.h"
//static bool isOpenSequencerWindow;

#include "resource.h"
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
    bool isOpenSequencerWindow;
    
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
//            style.Flags |= ImNodesStyleFlags_GridLinesPrimary | ImNodesStyleFlags_GridSnapping;
        // (optional) set browser properties
        ImGui::StyleColorsClassic();
        ImNodes::StyleColorsClassic();

        
        open_file_dialog = new ImGui::FileBrowser();
        open_file_dialog->SetTitle("open file");
        open_file_dialog->SetTypeFilters({ ".json" });
        
        save_file_dialog = new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
        save_file_dialog->SetTitle("save file");
        save_file_dialog->SetTypeFilters({ ".json" });
        
        my_audio_interface->init_devices(44100, 256);  //  sample rate, buffer size, input_device_id, output_device_id

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
//        clip_timeline_window(&isOpenSequencerWindow);
        
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
