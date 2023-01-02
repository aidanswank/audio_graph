#include <iostream>
#include <vector>
#include "SDL2/SDL.h"

#include "audio_interface.h"
#include "xmodule.h"
#include "midi_in_module.h"
#include "vst3_module.h"

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
        for(int i = 0; i < 256; i++)
        {
            xmodule::audio.push_back(0.0f);
        }
    }
    void process(std::vector<xmodule*>& modules) override
    {
    //        std::cout <<  "id " << id << " final output" << std::endl;
//        xmodule::audio.clear();
        
//        memcpy(audio.data(), 0.0f, sizeof(float)*256);
        for(int i = 0 ; i < 256; i++)
        {
            xmodule::audio[i] = 0.0f;
        }
        
        for(int i = 0; i < input_ids.size(); i++)
        {
            xmodule *mod = modules[ input_ids[i] ];
            for(int i = 0; i < mod->audio.size(); i++)
            {
                xmodule::audio[i] += mod->audio[i];
            }
//            mod->audio.clear();
//            std::cout <<  "summing " << i << " audio size " << mod->audio.size() << std::endl;
        }
    
    };
};

void DFS(int rootId, std::vector<xmodule*> &xmodules, std::vector<int> &visited)
{
    if (find(visited.begin(), visited.end(), rootId) != visited.end())
        return;                // Return if the node has already been visited
    visited.push_back(rootId); // Mark the node as visited
    // process the audio signal for the input xmodules of the current node
    for (int input_id : xmodules[rootId]->input_ids)
    {
        DFS(input_id, xmodules, visited);
    }
    xmodules[rootId]->process(xmodules); // process the audio signal for the current node
    // process the audio signal for the output xmodules of the current node
    for (int output_id : xmodules[rootId]->output_ids)
    {
        DFS(output_id, xmodules, visited); // Recursively process the audio signal for the output nodes
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
    DFS(3, interface->xmodules, interface->visited);

    float *output = (float*)outputBuffer;

    for( int i=0; i<framesPerBuffer; i++ )
    {
//        float white = (float)(rand() % 100)/100.0f;
//        output[i * 2] = white*0.01; /* left */
//        output[i * 2 + 1] = white*0.01;  /* right */
     
        output[i * 2] = interface->xmodules[3]->audio[i]; /* left */
        output[i * 2 + 1] = interface->xmodules[3]->audio[i];  /* right */
//        interface->xmodules[3]->audio.clear();
        
    }
    return 0;
}

int main()
{
    // Create a vector to store the xmodules
    std::vector<xmodule*> xmodules;
    
    rt_midi_in* midi_in = new rt_midi_in(0);
//    midi_in->something = 123456789;
    xmodules.push_back(midi_in); // rt midi in
    xmodules.push_back(new vst3_midi_instrument(1)); // vst plug
    xmodules.push_back(new vst3_midi_instrument(2)); // vst plug
    xmodules.push_back(new audio_output(3)); // output
    
    xmodules[0]->add_output(1);
    xmodules[0]->add_output(2);
    xmodules[1]->add_input(0);
    xmodules[1]->add_output(3);
    xmodules[2]->add_input(0);
    xmodules[2]->add_output(3);
    xmodules[3]->add_input(1);
    xmodules[3]->add_input(2);
    
    // Start the audio signal processing at the root node (in this case, the mixer xmodule)
    std::vector<int> visited;
//    DFS(3, xmodules, visited);
    
//    while(1) {
//        visited.clear();
//        DFS(3, xmodules, visited);
//    };
//
    
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    unsigned int w, h;
    w = 320 * 2;
    h = 240 * 2;
    window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetScale(renderer, 2, 2); // retina is 2x scale
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Couldn't initialize SDL: \n%s\n", SDL_GetError());
    }
    
    audio_interface interface;
    interface.scan_devices();
    interface.init_devices(44100, 256, 1, 2);  // place device indices here
    interface.pass_userdata(xmodules, visited);
    interface.turn_on(audio_callback);
    
    bool is_running = true;
    while (is_running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
           if (event.type == SDL_QUIT)
           {
               is_running = false;
           }
        }
//        visited.clear();
//        DFS(3, xmodules, visited);
    }

    return 0;
}