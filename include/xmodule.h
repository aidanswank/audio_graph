#pragma once

#include <vector>
#include "vprint.h"
#include "imnodes.h"
#include "graph.h"
#include <map>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// if null set zero
void check_and_load(json& obj, std::string key, void* data);

#include "midi.h"

// multichannel audio
typedef std::vector<std::vector<float>> STEREO_AUDIO;

void zero_audio(STEREO_AUDIO &audio, int len);

struct xmodule
{
    // Constructor
    // xmodule(int p_id, int p_num_inputs, int p_num_outputs) : id(p_id), num_inputs(p_num_inputs), num_ouputs(p_num_outputs) {}
    xmodule(audio_graph<xmodule*>& p_graph, ImVec2 click_pos) : graph(p_graph) {
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
            graph.attr2inslot[ graph.attr_counter ] = i;
//            print("attr",graph.attr_counter,"slot",i);
            
            std::vector<int> ids;
            ids.push_back(-1);
            input_ids.push_back(ids);
            
            graph.attr_counter++;
        }
        
        for(int i = 0; i < num_ouputs; i++)
        {
            output_attrs.push_back(graph.attr_counter);
            graph.attr2id[ graph.attr_counter ] = id;
            graph.attr2outslot[ graph.attr_counter ] = i;
//            print("attr",graph.attr_counter,"slot",i+num_inputs);
            
            std::vector<int> ids;
            ids.push_back(-1);
            output_ids.push_back(ids);

            graph.attr_counter++;
        }
    }

//    // Add an input to the xmodule
//    void add_input(int p_input_id)
//    {
//        input_ids.push_back(p_input_id);
//        print("[xmodule] id",id,"\tinput size ", input_ids.size(),"\tinput id  ",p_input_id,"\tid counter",graph.attr_counter);
//    }
    
//    void add_input_slot(int attr, int slot)
//    {
//        attr2slot[attr] = slot;
//    };

//    // Add an output to the xmodule
//    void add_output(int p_output_id)
//    {
//        output_ids.push_back(p_output_id);
//        print("[xmodule] id",id,"\toutput size", output_ids.size(),"\toutput id ",p_output_id,"\tid counter",graph.attr_counter);
//    }
    

    // preprocess
    // process the audio signal
    virtual void process() = 0;
    
    // show user interface
    virtual void show() = 0;
    
    virtual void save_state(nlohmann::json& object) = 0;
    virtual void load_state(nlohmann::json& object) = 0;


    // ~xmodule() {};

    int id;

    // The number of inputs and outputs for the xmodule
    int num_inputs;
    int num_ouputs;

    // The input and output xmodule IDs
//    std::vector<int> input_ids;
    std::vector<std::vector<int>> input_ids;
    std::vector<int> input_attrs;
    
//    std::vector<int> output_ids;
    std::vector<std::vector<int>> output_ids;
    std::vector<int> output_attrs;
    
//    std::map<int, int> attr2slot;
    
    //todo seperate
    std::vector<midi_note_message> input_notes;
    STEREO_AUDIO output_audio;
    STEREO_AUDIO input_audio;
    
    // if the user wants to store some data that isn't audio or midi
    void* input_void_ptr = NULL;
    void* output_void_ptr = NULL;
//    std::vector<xmodule*>& modules;
    audio_graph<xmodule*>& graph;
    std::string name;
//    std::vector<std::vector<float>> audio;
};

//// the types of the class factories
//typedef xmodule* create_t(audio_graph<xmodule*>& p_graph);
//typedef void destroy_t(xmodule*);
//create_t
