#include <iostream>
#include <vector>
#include "SDL2/SDL.h"

#include "xmodule.h"
#include "midi_in_module.h"
#include "vst3_module.h"
#include "cjfilter_module.h"
#include "audio_output_module.h"
#include "osc_module.h"
#include "midi_sequencer_module.h"
//#include "graph.h"
#include "audio_interface.h"
//#include "audio_callback.h"
#include "MidiFile.h"

global_transport g_transport;

#include "piano_roll.h"
static bool isOpenSequencerWindow;
smf::MidiFile mymidifile;

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
        g_transport.sample_count += framesPerBuffer;
        g_transport.ms_per_tick = get_ms_per_tick(g_transport.tempo,g_transport.ticks_per_quarter_note);
        float midi_tick_count = samples_to_ticks(g_transport.sample_count, g_transport.ms_per_tick, 44100);
        g_transport.midi_tick_count = midi_tick_count;
        g_transport.current_seconds = g_transport.sample_count/44100.0f;
    }
    
//    interface->graph->clear(); // clear previous search
//    interface->graph->DFS(root_node); // do graph search from root node
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
            output[i * 2] = graph->xmodules[root_node]->output_audio[0][i]; /* left */
            output[i * 2 + 1] = graph->xmodules[root_node]->output_audio[1][i];  /* right */
        }
    }

    return 0;
}

#include "im_wrap.h"

void audio_settings_gui(audio_interface* interface)
{
    static bool audio_settings_open = true;
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
                    interface->try_params();
                    interface->turn_on(audio_callback);
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

class user_interface
{
public:
    im_wrap ui;
    audio_graph<xmodule*>* graph;
    std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&)>* factory_map;
    audio_interface *my_audio_interface;

    user_interface(SDL_Window* window,
                   SDL_GLContext gl_context,
                   audio_graph<xmodule*>* p_graph,
                   std::map<std::string,
                   xmodule* (*)(audio_graph<xmodule*>&)>* p_factory_map,
                   audio_interface *p_audio_interface)
    {
        factory_map = p_factory_map;
        graph = p_graph;
        my_audio_interface = p_audio_interface;
        print("ui init");
        ui.init(window,gl_context);
        isOpenSequencerWindow=true;
    }
    ~user_interface()
    {
        print("ui shutdown");
        ui.shutdown();
    }
    void update()
    {
        
        ui.new_frame();
        
        audio_settings_gui(my_audio_interface);

//        ui.update();
        
        ImGui::ShowDemoWindow();
        
        piano_roll_window(&isOpenSequencerWindow, mymidifile);

        static bool node_editor_active = true;
        if(node_editor_active)
        {
            ImGui::Begin("patch editor", &node_editor_active);
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
                
                // loop through factory map to display names and push back if selected
                for (std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&)>::iterator it = factory_map->begin(); it != factory_map->end(); ++it) {
//                    std::cout << it->first << std::endl;
                    if (ImGui::MenuItem(it->first.c_str()))
                    {
//                        print(click_pos.x, click_pos.y, it->first.c_str());
                        xmodule* m = factory_map->at(it->first)(*graph);
                        if(m->name=="audio output")
                        {
                            graph->root_id = m->id;
                        }
                        graph->xmodules.push_back( m );
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

            ImNodes::EndNodeEditor();

            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                print("start", start_attr, "end", end_attr);
                print("arr2id map start", graph->attr2id[start_attr], "end", graph->attr2id[end_attr]);
        
                std::vector<int>& start_attr_vector  = graph->xmodules[ graph->attr2id[start_attr] ]->output_ids[ graph->attr2outslot[end_attr] ];
                
                std::vector<int>& end_attr_vector    = graph->xmodules[ graph->attr2id[end_attr] ]->input_ids[ graph->attr2inslot[end_attr] ];
                
                
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
//                        graph->
//                        graph2.erase_edge(edge_id);
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
    
//    im_wrap gui;
//    gui.init(window, gl_context);

    // graph that contains xmodule nodes
    
    audio_graph<xmodule*> graph;
    // idk just easier right now
    graph.event = &event;
    
    std::map<std::string, xmodule* (*)(audio_graph<xmodule*>&)> module_factory_map;
    
    module_factory_map[module_audio_output__get_name()]    = &module_audio_output__create;
    module_factory_map[module_midi_in__get_name()]         = &module_midi_in__create;
    module_factory_map[module_vst3_instrument__get_name()] = &module_vst3_instrument__create;
    module_factory_map[module_cjfilter__get_name()]        = &module_cjfilter__create;
    module_factory_map[module_osc__get_name()]             = &module_osc__create;
    module_factory_map[module_midi_sequencer__get_name()]  = &module_midi_sequencer__create;

//    graph.xmodules.push_back( factory_map[module_midi_in__get_name()](graph) ); // rt midi in
//    graph.xmodules.push_back( factory_map[module_vst3_instrument__get_name()](graph) ); // vst plug
//    graph.xmodules.push_back( factory_map[module_vst3_instrument__get_name()](graph) ); // vst plug
//    graph.xmodules.push_back( factory_map[module_audio_output__get_name()](graph) ); // output
//    graph.xmodules.push_back( factory_map[module_cjfilter__get_name()](graph) ); // filter
    
////     example patch
//    graph.xmodules[0]->add_output(1);
//    graph.xmodules[0]->add_output(2);
//
//    graph.xmodules[1]->add_input(0);
//    graph.xmodules[1]->add_output(4);//filter
//
//    graph.xmodules[2]->add_input(0);
//    graph.xmodules[2]->add_output(3);
//
//    graph.xmodules[3]->add_input(4);
//    graph.xmodules[3]->add_input(2);
//
//    graph.xmodules[4]->add_input(1);
//    graph.xmodules[4]->add_output(3);
//
//    std::pair<int, int> link{0, 1};
//    graph.links.push_back(link);
//
//    std::pair<int, int> link2{0, 3};
//    graph.links.push_back(link2);
//
//    std::pair<int, int> link3{2, 6};
//    graph.links.push_back(link3);
//
//    std::pair<int, int> link4{7, 5};
//    graph.links.push_back(link4);
//
//    std::pair<int, int> link5{4, 5};
//    graph.links.push_back(link5);
//
    
    
//
    // another one
//    graph.xmodules[0]->add_output(1);
//    graph.xmodules[0]->add_output(2);
//
//    graph.xmodules[1]->add_input(0);
//    graph.xmodules[1]->add_output(3);//filter
//
//    graph.xmodules[2]->add_input(0);
//    graph.xmodules[2]->add_output(3);
//
//    graph.xmodules[3]->add_input(1);
//    graph.xmodules[3]->add_input(2);
    
//    graph.DFS(3);

    // set up audio interface and open stream
    audio_interface interface;
    interface.scan_devices();
    interface.sample_rate=44100;
    interface.buffer_size=256;
//    interface.init_devices(44100, 256, 0, 1);  //  sample rate, buffer size, input_device_id, output_device_id
    interface.pass_userdata(&graph);
//    interface.turn_on(audio_callback);
    
//    smf::MidiFile mymidifile;
    int midifile_err = mymidifile.read("/Users/aidan/dev/cpp/dfs_modules/arp.mid");
    if (midifile_err == 0)
    {
        std::cout << "error loading midi!! :(" << std::endl;
    }
    g_transport.midifile=&mymidifile;
    mymidifile.linkNotePairs();
    
    user_interface ui(window, gl_context, &graph, &module_factory_map, &interface);
    
    bool is_running = true;
    
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
