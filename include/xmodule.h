#pragma once

#include <vector>
#include "vprint.h"
#include "imnodes.h"
#include "graph.h"

struct MidiNoteMessage
{
    int noteNum = -1;
    float velocity = 0.0f;
    bool isNoteOn = false;
};

// multichannel audio
typedef std::vector<std::vector<float>> STEREO_AUDIO;

void zero_audio(STEREO_AUDIO &audio, int len);

struct xmodule
{
    // Constructor
    // xmodule(int p_id, int p_num_inputs, int p_num_outputs) : id(p_id), num_inputs(p_num_inputs), num_ouputs(p_num_outputs) {}
    xmodule(int p_id, audio_graph<xmodule*>& p_graph) : id(p_id), graph(p_graph) {
        zero_audio(output_audio, 256);
        print("module id", p_id);
//        for(int i = 0; i < input_ids.size(); i++)
//        {
//            print("inputs", input_ids[i]);
//        }
    }

    // Add an input to the xmodule
    void add_input(int p_input_id)
    {
        input_ids.push_back(p_input_id);
//        std::pair<int, int> link{id, p_input_id};
//        graph.links.push_back(link);
    }

    // Add an output to the xmodule
    void add_output(int p_output_id)
    {
        output_ids.push_back(p_output_id);
        std::pair<int, int> link{id, p_output_id};
        graph.links.push_back(link);
    }

    // process the audio signal
    virtual void process() = 0;
    
    // show user interface
    virtual void show() = 0;
    
    // poll user input
    virtual void poll() = 0;

    // ~xmodule() {};

    int id;

    // The number of inputs and outputs for the xmodule
    int num_inputs;
    int num_ouputs;

    // The input and output xmodule IDs
    std::vector<int> input_ids;
    std::vector<int> output_ids;
    
    //todo seperate
    std::vector<MidiNoteMessage> notes;
    STEREO_AUDIO output_audio;
    STEREO_AUDIO input_audio;
    
    
//    std::vector<xmodule*>& modules;
    audio_graph<xmodule*>& graph;
    std::string name;
//    std::vector<std::vector<float>> audio;
};
