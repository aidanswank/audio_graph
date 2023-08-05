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

    for(int i = 0; i < len*2; i++)
    {
        mixed_stream[i]=0;
    }

for(int i = 0; i < voices.size(); i++) {
    if(voices[i].note.is_note_on) {
        for(int j = 0; j < len; j++)
        {
            midi_note_message event = voices[i].note;

            float t=(float)(g_transport.midi_tick_count-event.tick)/(float)event.duration;

            float freq_a = midi2freq(event.note_num+(event.pitch_bend_a));
            float freq_b = midi2freq(event.note_num+(event.pitch_bend_b));
            float lerp_freq = lerp(freq_a,freq_b,pow(t,2.0));
            
            float cosine_interp_freq = CosineInterpolate(freq_a, freq_b, t);
            voices[i].synth->setFrequency(lerp_freq);
            
            
            float samp = voices[i].synth->getAndInc();

            mixed_stream[j] += samp;
        }
    }
}
    
    for(int i = 0; i < len; i++)
    {
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

