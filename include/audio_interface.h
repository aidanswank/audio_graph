#pragma once
#include "vprint.h"
#include "portaudio.h"
#include <vector>
#include "xmodule.h"
//#include "graph.h"

class audio_interface
{
public:
    audio_interface();
    ~audio_interface();

    void init_devices(int sample_rate, int buffer_size);
    void scan_devices();
    void turn_on(PaStreamCallback* callback);
    void close_stream();
    void try_params();
    
////    std::vector<xmodule*> xmodules;
////    std::vector<int> visited;
////    std::vector<int> process_order;
//    audio_graph *graph;
////    test<float> *thing;
    void* data;
    void pass_userdata(void *p_data)
    {
//        graph = p_graph;
        data = p_data;
//        xmodules = p_xmodules;
//        visited = p_visited;
//        visited = p_process_order;
    };
    
    std::vector<PaDeviceInfo> device_infos;
    void set_param(bool is_capture, int dev_id);

//private:
    int sample_rate;
    int buffer_size;
    PaStreamParameters input_parameters;
    PaStreamParameters output_parameters;
    PaStream *stream;

};
