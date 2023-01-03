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

int main()
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    unsigned int w, h;
    w = 320 * 2;
    h = 240 * 2;
    window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetScale(renderer, 2, 2); // retina is 2x scale
//    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetVSync(renderer, 1);
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Couldn't initialize SDL: \n%s\n", SDL_GetError());
    }
    SDL_Event event;
    
    // graph that contains xmodule nodes
    audio_graph graph;
    
    graph.xmodules.push_back(new rt_midi_in(0)); // rt midi in
    graph.xmodules.push_back(new vst3_midi_instrument(1,&event)); // vst plug
    graph.xmodules.push_back(new vst3_midi_instrument(2,&event)); // vst plug
    graph.xmodules.push_back(new audio_output_module(3)); // output
    graph.xmodules.push_back(new cjfilter_module(4)); // filter

    // example patch
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

    // set up audio interface and open stream
    audio_interface interface;
    interface.scan_devices();
    interface.init_devices(44100, 256, 1, 2);  // place device indices here
    interface.pass_userdata(&graph);
    interface.turn_on(audio_callback);
    
    bool is_running = true;
    
    // main window loop
    while (is_running)
    {
        while(SDL_PollEvent(&event))
        {
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
        
        for(uint i = 0; i < graph.xmodules.size(); ++i)
        {
            graph.xmodules[i]->show();
        }
        
        // if i dont have this cpu spikes to 100%!
        //sdl2 vsync not working?? https://discourse.libsdl.org/t/high-cpu-usage/14676/20
        float this_tick = SDL_GetTicks();
        float next_tick = this_tick + (1000/60); // 60 fps
        if ( this_tick < next_tick )
        {
            SDL_Delay(next_tick-this_tick);
        }
    }
    
    SDL_Quit();

    return 0;
}
