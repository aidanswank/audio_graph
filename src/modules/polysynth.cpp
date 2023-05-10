#include "polysynth.h"

polysynth::polysynth(int voice_count)
{
    std::cout << "hello" << std::endl;
    buffer_size = 256;
    sample_rate = 44100;
    
    for(int i = 0; i < voice_count; i++)
    {
        synth_voice new_voice(sample_rate);
        voices.push_back(new_voice);
    }
    
    mixed_stream = new float[buffer_size*2];
}

int polysynth::get_active_voices()
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

// returns pointer to current free voice when new note is needed
int polysynth::find_free_voice()
{
//    PolyBLEP* free_voice = NULL;
//    PolyBLEP* last_active_voice = NULL;
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
//

extern global_transport g_transport;

void polysynth::generate_samples(float *stream, int len)
{
//    mixed_stream = new float[len*2]();
//    print("hello");
    for(int i = 0; i < len*2; i++)
    {
        mixed_stream[i]=0;
    }

    for(int i = 0; i < voices.size(); i++)
    {
        if(voices[i].note.is_note_on)
        {
//            voices[i].generateSamples(len);
//            blep->setFrequency(freq);
            
//            float samp = blep->getAndInc();
//            print("is active");


            for(int j = 0; j < len; j++)
            {
            
//                float bend = 0;
//                if(voices[i].slope!=0);
//                   bend = midi2freq(voices[i].slope);
                
//                voices[i].synth->setFrequency(voices[i].synth->getFreqInHz()+(voices[i].slope/10));
//                voices[i].synth->setFrequency(voices[i].synth->getFreqInHz());

//                print("samp",samp);
//                float ms_per_tick = get_ms_per_tick(120,96);
//                int samples = ms_per_tick * 44100.0 / 1000
//                print("bend",bend);
//                std::cout <<"sample"<<samp << std::endl;
//                voices[i].synth->set
                
                midi_note_message event = voices[i].note;
//                print(event.pitch_bend_a,event.pitch_bend_b,event.duration);
//                +samples_to_ticks(i,get_ms_per_tick(g_transport.tempo,96.0),44100)
                float t=(float)(g_transport.midi_tick_count-event.tick)/(float)event.duration;
//                print("midi tick count",g_transport.midi_tick_count);
//                print("t from ps",t);
//                print(t+samples_to_ticks(j,get_ms_per_tick(120,96.0),44100));
//                print(t);

                
//                print(event.pitch_bend_a,event.pitch_bend_b,lerp(midi2freq(event.pitch_bend_a),midi2freq(event.pitch_bend_b),t));
                float freq_a = midi2freq(event.note_num+(event.pitch_bend_a));
                float freq_b = midi2freq(event.note_num+(event.pitch_bend_b));
                float lerp_freq = lerp(freq_a,freq_b,pow(t,2.0));
//                print(freq_a,freq_b,lerp_freq);
//                                voices[i].synth->setFrequency(midi2freq(event.note_num)+freq);

                voices[i].synth->setFrequency(lerp_freq);
                
//                print(event.pitch_bend_a,event.pitch_bend_b,t);
                
                float samp = voices[i].synth->getAndInc();

                mixed_stream[j] += samp * 0.5;
//                mixed_stream[j*2+1] += samp * 0.5;

            }
        }
    }
    
    for(int i = 0; i < len; i++)
    {
//        print(samples_to_ticks(i,get_ms_per_tick(120,96.0),44100));

        stream[i] = mixed_stream[i];
    }
}

float calculate_slope(float a, float b, float duration) {
//    print("a",a,"b","dur",duration);
    return (b - a) / duration;
}

// takes a in midi messages
void polysynth::send_message(midi_note_message midi_message)
{
//    int nBytes = midiMessage.size();

    if(midi_message.is_note_on) // note on
    {
        int freevoice_idx = find_free_voice();

//            int numActiveVoices = getActiveVoices();
//            std::cout << numActiveVoices << std::endl;

        if(freevoice_idx!=-1)
        {
//                freevoice->phaseReset=true;
//                freevoice->isActive=true;
//                freevoice->durationPlayed=0;
//                freevoice->velocity = float(midiMessage[2])/127.0;
//                std::cout << "velocityyyyy " << freevoice->velocity << std::endl;
//                freevoice->volume = freevoice->velocity;
////                std::cout << freevoice->A << std::endl;
//                freevoice->note = (int)midiMessage[1];
//                freevoice->pitch = midi2Freq(((int)midiMessage[1])+24)*1;
            
            synth_voice &nvoice = voices[ freevoice_idx ];
            nvoice.synth->setFrequency(midi2freq(midi_message.note_num));
            nvoice.synth->sync(0.0);
//            nvoice.slope = 0;
            nvoice.note = midi_message;
            nvoice.freq = midi2freq(midi_message.note_num);
//            nvoice.note.is_note_on = true;
//            nvoice.note.note_num = midi_message.note_num;
//            nvoice.slope = calculate_slope(midi_message.pitch_bend_a,midi_message.pitch_bend_b,midi_message.duration);
//            print("note on");
//            print("slope!!",nvoice.slope,"a b dur",midi_message.pitch_bend_a,midi_message.pitch_bend_b,midi_message.duration);
        }

    }

    if(!midi_message.is_note_on) // note off
    {

//        print("note off!");
        for(int i = 0; i < voices.size(); i++)
        {
            // std::cout << i << "v " << voices[i].osc.isActive << " " << &voices[i].osc.isActive << std::endl;
            if( ( midi_message.note_num==voices[i].note.note_num ) && ( voices[i].note.is_note_on ) )
            {
//                    voices[i].release();
//                print("release!");
                voices[i].note.is_note_on = false;
//                voices[i].slope = 0;
//                voices[i].note.note_num = -1;
            }
        }
        int num_active_voices = get_active_voices();
//        std::cout << "active voices" << num_active_voices << std::endl;
    }
}

