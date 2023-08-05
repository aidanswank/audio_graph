#include <iostream>
#include <vector>
#include "SDL2/SDL.h"

#include "global_transport.h"
global_transport g_transport;
// modules
#include "xmodule.h"
#include "midi_in_module.h"
//#include "vst3_module.h"
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
#include "allpass_module.h"
#include "tapedelay_module.h"
#include "biquad_module.h"

//#include "graph.h"
#include "audio_interface.h"
//#include "audio_callback.h"
#include "MidiFile.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
json current_patch;

#include "user_interface.h"

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
//    module_factory_map[module_vst3_instrument__get_name()] = &module_vst3_instrument__create;
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
    module_factory_map[module_allpass__get_name()]         = &module_allpass__create;
    module_factory_map[module_tapedelay__get_name()]       = &module_tapedelay__create;
    module_factory_map[module_biquad__get_name()]          = &module_biquad__create;

    // set up audio interface and open stream
    audio_interface interface;
    interface.scan_devices();
    interface.pass_userdata(&graph);
    
    user_interface ui(window, gl_context, &graph, &module_factory_map, &interface);
//    ui.load_patch("start_patch.json");
    
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
//            for(uint i = 0; i < graph.xmodules.size(); ++i)
//            {
//                if(graph.xmodules[i]->name=="vst instrument") // sdl2 polling for easyvst
//                {
//                    vst3_midi_instrument* vst_mid_inst_ptr = (vst3_midi_instrument*)graph.xmodules[i];
//                    vst_mid_inst_ptr->poll();
//                }
//            }
        }
                
        ui.update();
        
    }
    
//    gui.shutdown();
    SDL_Quit();

    return 0;
}
