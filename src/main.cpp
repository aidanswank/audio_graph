#include <iostream>
#include <vector>
#include "SDL2/SDL.h"

#include "xmodule.h"
#include "midi_in_module.h"
#include "vst3_module.h"
#include "cjfilter_module.h"
#include "audio_output_module.h"
#include "audio_interface.h"
//#include "audio_callback.h"

static int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{

    audio_interface* interface = (audio_interface*)userData;

    int root_node = 3;
    
    interface->graph->clear(); // clear previous search
    interface->graph->DFS(root_node); // do graph search from root node

    float *output = (float*)outputBuffer;

    for( uint i=0; i<framesPerBuffer; ++i )
    {
        // TODO CHANGE ROOT NODE TO 0 NOT 3 !!
        output[i * 2] = interface->graph->xmodules[root_node]->output_audio[0][i]; /* left */
        output[i * 2 + 1] = interface->graph->xmodules[root_node]->output_audio[1][i];  /* right */
    }
    
    return 0;
}

#include "im_wrap.h"

class user_interface
{
public:
    im_wrap ui;
    audio_graph* graph;
    
    user_interface(SDL_Window* window, SDL_GLContext gl_context, audio_graph* p_graph)
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
        
//        ui.update();
        
        const int hardcoded_node_id = 1;

        static bool node_editor_active = true;
        if(node_editor_active)
        {
            ImGui::Begin("testing", &node_editor_active, ImGuiWindowFlags_MenuBar);
            ImNodes::BeginNodeEditor();
            
//            ImNodes::BeginNode(hardcoded_node_id);
//            ImGui::Dummy(ImVec2(80.0f, 45.0f));
//            ImNodes::EndNode();
            
            for(uint i = 0; i < graph->xmodules.size(); ++i)
            {
                graph->xmodules[i]->show();
            }
            
            ImNodes::EndNodeEditor();
            
            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
//              links.push_back(std::make_pair(start_attr, end_attr));
                print("start", start_attr, "end", end_attr);
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
//    SDL_Window *window = nullptr;
//    SDL_Renderer *renderer = nullptr;
//    unsigned int w, h;
//    w = 320 * 2;
//    h = 240 * 2;
//    window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
//    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
////    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
//    SDL_RenderSetScale(renderer, 2, 2); // retina is 2x scale
////    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//    SDL_RenderSetVSync(renderer, 1);
//    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
//    {
//        printf("Couldn't initialize SDL: \n%s\n", SDL_GetError());
//    }
    
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
    
    audio_graph graph;

    graph.xmodules.push_back(new rt_midi_in(0,graph.xmodules)); // rt midi in
    graph.xmodules.push_back(new vst3_midi_instrument(1,&event,graph.xmodules)); // vst plug
    graph.xmodules.push_back(new vst3_midi_instrument(2,&event,graph.xmodules)); // vst plug
    graph.xmodules.push_back(new audio_output_module(3,graph.xmodules)); // output
    graph.xmodules.push_back(new cjfilter_module(4,graph.xmodules)); // filter

    
    std::vector<std::pair<int, int>> links;

    // example patch
    graph.xmodules[0]->add_output(1);
    graph.xmodules[0]->add_output(2);
    std::pair<int, int> p1{0, 1};
    links.push_back(p1);
    std::pair<int, int> p2{0, 2};
    links.push_back(p2);
    graph.xmodules[1]->add_input(0);
    graph.xmodules[1]->add_output(4);//filter
    graph.xmodules[2]->add_input(0);
    graph.xmodules[2]->add_output(3);
    graph.xmodules[3]->add_input(4);
    graph.xmodules[3]->add_input(2);
    graph.xmodules[4]->add_input(1);
    graph.xmodules[4]->add_output(3);

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
        
//        gui.new_frame();
//        gui.update();
//        gui.render();
        ui.update();
        
//        for(uint i = 0; i < graph.xmodules.size(); ++i)
//        {
//            graph.xmodules[i]->show();
//        }

        //        // if i dont have this cpu spikes to 100%!
//        //sdl2 vsync not working?? https://discourse.libsdl.org/t/high-cpu-usage/14676/20
//        float this_tick = SDL_GetTicks();
//        float next_tick = this_tick + (1000/60); // 60 fps
//        if ( this_tick < next_tick )
//        {
//            SDL_Delay(next_tick-this_tick);
//        }
    }
    
//    gui.shutdown();
    SDL_Quit();

    return 0;
}
