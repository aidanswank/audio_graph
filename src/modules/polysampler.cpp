#include "polysampler.h"

polysampler::polysampler(int voice_count)
{
    std::cout << "hello" << std::endl;
    buffer_size = 256;
    sample_rate = 44100;
    
    
    mixed_stream = new float[buffer_size*2];
    
//    load_sample("/Users/aidan/dev/cpp/dfs_modules/build/Debug/Roland\ TR-909/BT7AADA.WAV");
    
    for(int i = 0; i < voice_count; i++)
    {
        sampler_voice new_voice(audio_info, &sample_buffer[0]);
        voices.push_back(new_voice);
    }
}

int polysampler::get_active_voices()
{
    int num = 0;
    for(int i = 0; i < voices.size(); i++)
    {
        if(voices[i].note.is_note_on)
        {
            num++;
        }
    }
    return num;
}

int polysampler::find_free_voice()
{
    int free_voice_idx = -1;
    int last_active_voice_idx = -1;
    
    for(int i = 0; i < voices.size(); i++)
    {
        if(!voices[i].note.is_note_on)
        {
            free_voice_idx = i;
            break;
        } else {
            last_active_voice_idx = i;
        }
    }
    
    if(free_voice_idx==-1)
    {
        free_voice_idx = last_active_voice_idx;
    }
    
    return free_voice_idx;
}

extern global_transport g_transport;

void polysampler::generate_samples(float *stream, int len)
{

    for(int i = 0; i < len*2; i++)
    {
        mixed_stream[i]=0;
    }

    if(is_loaded)
    {
        for(int i = 0; i < voices.size(); i++)
        {
            if(voices[i].note.is_note_on)
            {
                
                for(int j = 0; j < len; j++)
                {
                    midi_note_message event = voices[i].note;
                    float t=(float)(g_transport.midi_tick_count-event.tick)/(float)event.duration;
                    float freq_a = midi2speed(event.note_num+(event.pitch_bend_a));
                    float freq_b = midi2speed(event.note_num+(event.pitch_bend_b));
                    float lerp_freq = lerp(freq_a,freq_b,pow(t,3.0));
                    //
                    //                voices[i].synth->setFrequency(lerp_freq);
                    //
                    //                float samp = voices[i].synth->getAndInc();
                    
                    //                float samp =
                    voices[i].speed = lerp_freq;
                    float samp = voices[i].get_and_inc();
                    
                    mixed_stream[j] += samp;
                    
                }
            }
        }
    }
    
    for(int i = 0; i < len; i++)
    {
//        print(samples_to_ticks(i,get_ms_per_tick(120,96.0),44100));

        stream[i] = mixed_stream[i];
    }
}

// takes a in midi messages
void polysampler::send_message(midi_note_message midi_message)
{

    if(midi_message.is_note_on) // note on
    {
        int freevoice_idx = find_free_voice();

        if(freevoice_idx!=-1)
        {
            sampler_voice &nvoice = voices[ freevoice_idx ];
//            nvoice.synth->setFrequency(midi2freq(midi_message.note_num));
//            nvoice.synth->sync(0.0);
            nvoice.note = midi_message;
            nvoice.speed = midi2speed(midi_message.note_num);
            nvoice.current_playhead_pos = 0;
//            nvoice.freq = midi2freq(midi_message.note_num);
        }

    }

    if(!midi_message.is_note_on) // note off
    {

        for(int i = 0; i < voices.size(); i++)
        {
            if( ( midi_message.note_num==voices[i].note.note_num ) && ( voices[i].note.is_note_on ) )
            {

                voices[i].note.is_note_on = false;

            }
        }
        int num_active_voices = get_active_voices();
    }
}

