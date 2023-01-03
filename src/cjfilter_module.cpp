#include "cjfilter_module.h"

cjfilter_module::cjfilter_module(int id) : xmodule(id)
{
    cutoff = 0.1;
    resonance = 0.0;
}

void cjfilter_module::process(std::vector<xmodule *> &modules)
{
    xmodule* input_module = (xmodule*)modules[ input_ids[0] ];
    STEREO_AUDIO input_audio = input_module->audio;

    for(int i = 0; i < 256; ++i)
    {
        filter[0].doFilter(input_audio[0][i], cutoff, resonance);
        filter[1].doFilter(input_audio[1][i], cutoff, resonance);
        xmodule::audio[0][i] = filter[0].filterOut[0];
        xmodule::audio[1][i] = filter[1].filterOut[0]; //todo add another cjfilter class n do stereo
    }
}
