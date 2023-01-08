#include <iostream>
#include <vector>
#include "SDL2/SDL.h"

#include "xmodule.h"
#include "midi_in_module.h"
#include "vst3_module.h"
#include "cjfilter_module.h"
#include "audio_output_module.h"
//#include "graph.h"
#include "audio_interface.h"
//#include "audio_callback.h"

static int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{

    audio_interface* interface = (audio_interface*)userData;
    audio_graph<xmodule*>* graph = (audio_graph<xmodule*>*)interface->data;

    int root_node = 3;

//    interface->graph->clear(); // clear previous search
//    interface->graph->DFS(root_node); // do graph search from root node
    graph->clear();
    graph->DFS(3);

    float *output = (float*)outputBuffer;

    for( uint i=0; i<framesPerBuffer; ++i )
    {
//        float w = (float)(rand()%100)/1000.0f; //testing with white noise
        // TODO CHANGE ROOT NODE TO 0 NOT 3 !!
//        output[i * 2] = w;
//        output[i * 2 + 1] = w;  /* right */
        output[i * 2] = graph->xmodules[root_node]->output_audio[0][i]; /* left */
        output[i * 2 + 1] = graph->xmodules[root_node]->output_audio[1][i];  /* right */
    }

    return 0;
}

#include "im_wrap.h"

class user_interface
{
public:
    im_wrap ui;
    audio_graph<xmodule*>* graph;

    user_interface(SDL_Window* window, SDL_GLContext gl_context, audio_graph<xmodule*>* p_graph)
    {
        graph = p_graph;
        print("ui init");
        ui.init(window,gl_context);
    }
    ~user_interface()
    {
        print("ui shutdown");
        ui.shutdown();
    }
    void update()
    {
        ui.new_frame();

        ui.update();

        static bool node_editor_active = true;
        if(node_editor_active)
        {
            ImGui::Begin("testing", &node_editor_active, ImGuiWindowFlags_MenuBar);
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
                
                for(int i = 0; i < graph->module_names.size(); i++)
                {
                    if (ImGui::MenuItem(graph->module_names[i].c_str()))
                    {
                        print(click_pos.x, click_pos.y, graph->module_names[i]);
                    }
                }
                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

//            ImNodes::BeginNode(hardcoded_node_id);
//            ImGui::Dummy(ImVec2(80.0f, 45.0f));
//            ImNodes::EndNode();

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
                print("attr2id",graph->attr2id(start_attr),graph->attr2id(end_attr));
                graph->links.push_back(std::make_pair(start_attr, end_attr));
            }

            ImGui::End();
        }

        // Create a window called "My First Tool", with a menu bar.
        static bool debug_active = true;
        if(debug_active)
        {
            ImGui::Begin("debug", &debug_active);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                           1000.0f / ImGui::GetIO().Framerate,
                           ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ui.render();
    }
};

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
    
    vst3_midi_instrument* frozen;
    frozen->event;
//    frozen->init(graph);

    graph.xmodules.push_back(new rt_midi_in(graph)); // rt midi in
    graph.xmodules.push_back(frozen->init(graph)); // vst plug
    graph.xmodules.push_back(frozen->init(graph)); // vst plug
    graph.xmodules.push_back(new audio_output_module(graph)); // output
    graph.xmodules.push_back(new cjfilter_module(graph)); // filter

    
//    std::vector<std::pair<int, int>> links;

//     example patch
    graph.xmodules[0]->add_output(1);
    graph.xmodules[0]->add_output(2);

    graph.xmodules[1]->add_input(0);
    graph.xmodules[1]->add_output(4);//filter

    graph.xmodules[2]->add_input(0);
    graph.xmodules[2]->add_output(3);

    graph.xmodules[3]->add_input(4);
    graph.xmodules[3]->add_input(2);

    graph.xmodules[4]->add_input(1);
    graph.xmodules[4]->add_output(3);
    
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
    interface.init_devices(44100, 256, 2, 3);  // place device indices here
    interface.pass_userdata(&graph);
    interface.turn_on(audio_callback);
    
    user_interface ui(window, gl_context, &graph);
    
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
                graph.xmodules[i]->poll();
            }
        }
                
        ui.update();
        
    }
    
//    gui.shutdown();
    SDL_Quit();

    return 0;
}
