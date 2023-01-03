#include <iostream>
#include <vector>
#include "SDL2/SDL.h"

#include "audio_interface.h"
#include "xmodule.h"
#include "midi_in_module.h"
#include "vst3_module.h"
#include "cjfilter_module.h"

//
//struct vst_plugin : xmodule {
//    vst_plugin(int id) : xmodule(id) {}
//    void process(std::vector<xmodule*>& modules) override
//    {
////        std::cout <<  "id " << id << " vst plugin process" << std::endl;
////        rt_midi_in *midi_in = (rt_midi_in *)modules[input_ids[0]];
////        std::cout << "something!!!!!" << midi_in->something << std::endl;
//
////        for(int i = 0; i < input_ids.size(); i++)
////        {
////            std::cout << "input id " << input_ids[i] << std::endl;
////        }
//    };
//};

struct audio_output : xmodule {
    
    audio_output(int id) : xmodule(id)
    {
//        for(int i = 0; i < 256; i++)
//        {
//            xmodule::audio.push_back(0.0f);
//        }
        zero_audio(xmodule::audio,256);
    }
    
    void show() override {};
    void poll() override {};
    
    void process(std::vector<xmodule*>& modules) override
    {
    //        std::cout <<  "id " << id << " final output" << std::endl;
//        xmodule::audio.clear();
        
//        memset(xmodule::audio.data(), 0.0f, sizeof(float)*256);
        zero_audio(xmodule::audio,256);
        
        for(int i = 0; i < input_ids.size(); i++)
        {
            xmodule *mod = modules[ input_ids[i] ];
            // TODO CHANGE FIXED BUFFER
            for(int i = 0; i < 256; i++)
            {
//                xmodule::audio[i] += mod->audio[i];
                xmodule::audio[0][i] += mod->audio[0][i];
                xmodule::audio[1][i] += mod->audio[1][i];
            }
//            mod->audio.clear();
//            std::cout <<  "summing " << i << " audio size " << mod->audio.size() << std::endl;
        }
    
    };
};

void DFS(int rootId, std::vector<xmodule*> &xmodules, std::vector<int> &visited, std::vector<int> &process_order)
{
    if (find(visited.begin(), visited.end(), rootId) != visited.end())
        return;                // Return if the node has already been visited
    visited.push_back(rootId); // Mark the node as visited
    // process the audio signal for the input xmodules of the current node
    for (int input_id : xmodules[rootId]->input_ids)
    {
        DFS(input_id, xmodules, visited, process_order);
    }
    xmodules[rootId]->process(xmodules); // process the audio signal for the current node
    process_order.push_back(rootId);
    // process the audio signal for the output xmodules of the current node
    for (int output_id : xmodules[rootId]->output_ids)
    {
        DFS(output_id, xmodules, visited, process_order); // Recursively process the audio signal for the output nodes
    }
}

static int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{

    audio_interface* interface = (audio_interface*)userData;
//    interface
    
    interface->visited.clear();
    interface->process_order.clear();
    DFS(3, interface->xmodules, interface->visited, interface->process_order);
    
//    for(uint i = 0; i < interface->process_order.size(); ++i)
//    {
//        interface->xmodules[i]->process(interface->xmodules);
//    }

    float *output = (float*)outputBuffer;

    for( uint i=0; i<framesPerBuffer; ++i )
    {
//        float white = (float)(rand() % 100)/100.0f;
//        output[i * 2] = white*0.01; /* left */
//        output[i * 2 + 1] = white*0.01;  /* right */
     
//        uint num_modules = (uint)interface->xmodules.size() - 1;
        // TODO CHANGE ROOT NODE TO 0 NOT 3 !!
        output[i * 2] = interface->xmodules[3]->audio[0][i]; /* left */
        output[i * 2 + 1] = interface->xmodules[3]->audio[1][i];  /* right */
//        interface->xmodules[3]->audio.clear();
        
    }
    return 0;
}

int main()
{
    SDL_Event event;
    
    // Create a vector to store the xmodules
    std::vector<xmodule*> xmodules;
    
    rt_midi_in* midi_in = new rt_midi_in(0);
//    midi_in->something = 123456789;
    xmodules.push_back(midi_in); // rt midi in
    xmodules.push_back(new vst3_midi_instrument(1,&event)); // vst plug
    xmodules.push_back(new vst3_midi_instrument(2,&event)); // vst plug
    xmodules.push_back(new audio_output(3)); // output
    xmodules.push_back(new cjfilter_module(4)); // filter

    xmodules[0]->add_output(1);
    xmodules[0]->add_output(2);
    xmodules[1]->add_input(0);
    xmodules[1]->add_output(4);//filter
    xmodules[2]->add_input(0);
    xmodules[2]->add_output(3);
    xmodules[3]->add_input(4);
    xmodules[3]->add_input(2);
    xmodules[4]->add_input(1);
    xmodules[4]->add_output(3);

    // Start the audio signal processing at the root node (in this case, the mixer xmodule)
    std::vector<int> visited;
    std::vector<int> process_order;
//    DFS(3, xmodules, visited, process_order);
    
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
//    
    audio_interface interface;
    interface.scan_devices();
    interface.init_devices(44100, 256, 1, 2);  // place device indices here
    interface.pass_userdata(xmodules, visited, process_order);
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
            for(uint i = 0; i < xmodules.size(); ++i)
            {
                xmodules[i]->poll();
            }
        }
        
        for(uint i = 0; i < xmodules.size(); ++i)
        {
            xmodules[i]->show();
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
