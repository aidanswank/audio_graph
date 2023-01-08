#include "cjfilter_module.h"

cjfilter_module::cjfilter_module(audio_graph<xmodule*>& graph) : xmodule(graph)
{
//    name = "cjfilter";
//    graph.push_unique_str(name);
    config("cjfilter",1,1);
    cutoff = 0.1;
    resonance = 0.0;
}

void cjfilter_module::process()
{
    xmodule* input_module = (xmodule*)graph.xmodules[ input_ids[0] ];
    STEREO_AUDIO input_audio = input_module->output_audio;

    for(int i = 0; i < 256; ++i)
    {
        filter[0].doFilter(input_audio[0][i], cutoff, resonance);
        filter[1].doFilter(input_audio[1][i], cutoff, resonance);
        xmodule::output_audio[0][i] = filter[0].filterOut[0];
        xmodule::output_audio[1][i] = filter[1].filterOut[0]; //todo add another cjfilter class n do stereo
    }
}
