#pragma once

#include <vector>

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
    xmodule(int p_id) : id(p_id) {}

    // Add an input to the xmodule
    void add_input(int p_input_id)
    {
        input_ids.push_back(p_input_id);
    }

    // Add an output to the xmodule
    void add_output(int p_output_id)
    {
        output_ids.push_back(p_output_id);
    }

    // process the audio signal
    virtual void process(std::vector<xmodule*>& modules) = 0;
    
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
    STEREO_AUDIO audio;
//    std::vector<std::vector<float>> audio;
};
