#include "global_transport.h"

float get_ms_per_tick(int bpm, int ticks_per_quarter_note)
{
    float ms = 60000 / (float)(bpm * ticks_per_quarter_note);
    return ms;
}

float samples_to_ticks(int current_sample, float ms_per_tick, int sample_rate)
{
    return (1000*current_sample)/(ms_per_tick*sample_rate);
}
