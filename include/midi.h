#pragma once
#include "vprint.h"

#include <math.h>
double midi2freq(int n);

struct midi_note_message
{
    int note_num = -1;
    float velocity = 0.0f;
    bool is_note_on = false;
    int tick=-1;
    int key=-1;
    int duration=0;
    int pitch_bend_a=0;
    int pitch_bend_b=0;
    int linked_tick=-1;
    
    midi_note_message* linked_event = nullptr;
    
    int get_duration()
    {
        assert(linked_event!=nullptr);
        return abs(tick - linked_event->tick);
    }
    
    void set_duration()
    {
        if(get_duration()>1000)
        {
            print("something wrong");
            print(duration);
            print(linked_event->duration);
        } else {
            duration = get_duration();
        }
    }
};
