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
    xmodule(audio_graph<xmodule*>& p_graph) : graph(p_graph) {
        zero_audio(output_audio, 256);
        id = graph.id_counter;
        graph.id_counter++;
//        id = p_id;
        
        print("xmodule id", id);

    }
    
    void config(int p_num_inputs, int p_num_outputs)
    {
//        name = p_name;
//        graph.push_unique_str(name);
        num_inputs = p_num_inputs;
        num_ouputs = p_num_outputs;
        
        for(int i = 0; i < num_inputs; i++)
        {
            input_attrs.push_back(graph.attr_counter);
            graph.attr2id[ graph.attr_counter ] = id;
            graph.attr_counter++;
        }
        
        for(int i = 0; i < num_ouputs; i++)
        {
            output_attrs.push_back(graph.attr_counter);
            graph.attr2id[ graph.attr_counter ] = id;
            graph.attr_counter++;
        }
    }

    // Add an input to the xmodule
    void add_input(int p_input_id)
    {
        input_ids.push_back(p_input_id);
//        input_attrs.push_back(graph.id_counter);
        print("[xmodule] id",id,"\tinput size ", input_ids.size(),"\tinput id  ",p_input_id,"\tid counter",graph.attr_counter);
//        std::pair<int, int> link{id, p_input_id};
//        graph.links.push_back(link);
//        std::pair<int, int> link{input_attrs[0], output_attrs[0]};
//        graph.links.push_back(link);

        ///
//        graph.id_counter++;
    }

    // Add an output to the xmodule
    void add_output(int p_output_id)
    {
        output_ids.push_back(p_output_id);
//        output_attrs.push_back(graph.id_counter);
//        std::pair<int, int> link{id, p_output_id};
//        graph.links.push_back(link);
//        print(input_attrs[0], output_attrs[0]);
//        std::pair<int, int> link{input_attrs[input_ids.size()-1], output_attrs[output_ids.size()-1]};
//        graph.links.push_back(link);
        print("[xmodule] id",id,"\toutput size", output_ids.size(),"\toutput id ",p_output_id,"\tid counter",graph.attr_counter);
//        graph.id_counter++;
    }

    // process the audio signal
    virtual void process() = 0;
    
    // show user interface
    virtual void show() = 0;

    // ~xmodule() {};

    int id;

    // The number of inputs and outputs for the xmodule
    int num_inputs;
    int num_ouputs;

    // The input and output xmodule IDs
    std::vector<int> input_ids;
    std::vector<int> output_ids;
    
    std::vector<int> input_attrs;
    std::vector<int> output_attrs;
    
    //todo seperate
    std::vector<MidiNoteMessage> input_notes;
    STEREO_AUDIO output_audio;
    STEREO_AUDIO input_audio;
    
    void* input_void_ptr;
    
//    std::vector<xmodule*>& modules;
    audio_graph<xmodule*>& graph;
    std::string name;
//    std::vector<std::vector<float>> audio;
};

//// the types of the class factories
//typedef xmodule* create_t(audio_graph<xmodule*>& p_graph);
//typedef void destroy_t(xmodule*);
//create_t
