#include "audio_callback.h"

int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{

    audio_interface* interface = (audio_interface*)userData;
    audio_graph<xmodule*>* graph = (audio_graph<xmodule*>*)interface->data;

    if(g_transport.is_playing)
    {
    if(graph->xmodules.size()>0 && graph->root_id!=-1)
    {
        int root_node = graph->root_id;
        
        graph->clear();

        graph->DFS(root_node);
        
        float *output = (float*)outputBuffer;
        
        for( uint i=0; i<framesPerBuffer; ++i )
        {
            //        float w = (float)(rand()%100)/1000.0f; //testing with white noise
            //        output[i * 2] = w;
            //        output[i * 2 + 1] = w;  /* right */
            output[i * 2]       = graph->xmodules[root_node]->output_audio[0][i]; /* left */
            output[i * 2 + 1]   = graph->xmodules[root_node]->output_audio[1][i];  /* right */
        }
    }
    
    // have to increment sample count after module processing or we skip first note
        
        g_transport.ms_per_tick = get_ms_per_tick(g_transport.tempo,g_transport.ticks_per_quarter_note);
        float midi_tick_inc = samples_to_ticks(framesPerBuffer, g_transport.ms_per_tick, 44100);
        g_transport.ticks_per_sample = samples_to_ticks(1, g_transport.ms_per_tick, 44100);
        g_transport.midi_tick_count += midi_tick_inc;
        //        print("tick counter",g_transport.midi_tick_count);
        g_transport.sample_count += framesPerBuffer;
        g_transport.current_seconds = g_transport.sample_count/44100.0f;
        
        
        if(g_transport.midi_tick_count>=96*4)
        {
            g_transport.midi_tick_count=0;
            g_transport.current_seconds=0;
            g_transport.current_pattern++;
            if(g_transport.current_pattern>=4)
            {
                g_transport.current_pattern=0;
            }
//            print(g_transport.current_pattern);
        }
        
    }

    return 0;
}
