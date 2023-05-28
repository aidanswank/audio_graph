#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "vendor/PolyBLEP.h"
#include "midi.h"
#include "global_transport.h"
#include "interpolation.h"
#include <sndfile.h>

struct sampler_voice
{
    midi_note_message note;
//    PolyBLEP *synth;
    float freq=-1;
    
    SF_INFO audio_info;
    float* sample_data;
    
    float speed = 0.0;
    float current_playhead_pos = 0.0;

    sampler_voice(SF_INFO p_audio_info, float* p_sample_data)
    {
        sample_data = p_sample_data;
        audio_info = p_audio_info;
        note.is_note_on = false;
        note.note_num = -1;
        current_playhead_pos = 0.0;
        speed = 0.0;
    }
    
    float get_and_inc()
    {
                
        float sample = 0;
        if(current_playhead_pos>=audio_info.frames)
        {
            current_playhead_pos = 0;
//            sample = 0;
        } else {
            sample = sample_data[ (int)current_playhead_pos*audio_info.channels ];
            current_playhead_pos += speed;
        }
        
        return sample;
    }
    
};

// for load single cycle waveforms or other samples
class polysampler
{
public:
    std::vector<sampler_voice> voices;
    std::string filepath;
    bool is_loaded;

    float *mixed_stream;

    int buffer_size;
    int sample_rate;
    
    SF_INFO audio_info;
    std::vector<float> sample_buffer;

    polysampler(int voice_count);
    void load_sample(std::string p_filepath)
    {
        SNDFILE *audio_file;
        SF_INFO audio_info;
        filepath = p_filepath;
        is_loaded = true;
        
        audio_file = sf_open(p_filepath.c_str(), SFM_READ, &audio_info);
        if (audio_file == NULL) {
            printf("Error opening audio file: %s\n", sf_strerror(NULL));
            return 1;
        }
        print("frames",audio_info.frames);
        
        // Determine the size of the audio data vector
        sf_count_t num_frames = audio_info.frames;
        int num_channels = audio_info.channels;
        sample_buffer.resize(num_frames * num_channels);
        
        int num_read = sf_readf_float(audio_file, &sample_buffer[0], num_frames);
//        print("num read",num_read);
//        sf_close(audio_file);
        print("channels", audio_info.channels);
        print("sample rate", audio_info.samplerate);
        
        for(int i =0; i < voices.size(); i++)
        {
            voices[i].sample_data = &sample_buffer[0];
            voices[i].audio_info  = audio_info;

        }

//        for(int i = 0; i < 20; i+= audio_info.channels)
//        {
//            printf("Left channel: %f\tRight channel: %f\n", sample_buffer[i], sample_buffer[i+1]);
//        }
    }
    void send_message(midi_note_message midi_message);
    int get_active_voices();
    int find_free_voice();
    void generate_samples(float *stream, int len);
};
