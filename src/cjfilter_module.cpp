#include "cjfilter_module.h"

cjfilter_module::cjfilter_module(audio_graph<xmodule*>& graph) : xmodule(graph)
{
//    name = "cjfilter";
//    graph.push_unique_str(name);
    name = module_cjfilter__get_name();
    config(2,1);
    cutoff = 0.1;
    resonance = 0.0;
    
    input_void_ptr = new float[256](); // some mono signal to control cutoff
}

void cjfilter_module::process()
{
    if(input_ids.size()>0)
    {
        xmodule* input_module = (xmodule*)graph.xmodules[ input_ids[0] ];
        
        STEREO_AUDIO input_audio = input_module->output_audio;
        
        if(input_ids.size()>1)
        {
            xmodule* input2_test = (xmodule*)graph.xmodules[ input_ids[1] ];
//            for(int i = 0; i < input2_test->output_audio[0].size(); i++)
//            {
//                print("yup",input2_test->output_audio[0][i]);
//            }
        }

        for(int i = 0; i < 256; ++i)
        {
            filter[0].doFilter(input_audio[0][i], cutoff, resonance);
            filter[1].doFilter(input_audio[1][i], cutoff, resonance);
            xmodule::output_audio[0][i] = filter[0].filterOut[0];
            xmodule::output_audio[1][i] = filter[1].filterOut[0]; //todo add another cjfilter class n do stereo
        }
    }
}

xmodule* module_cjfilter__create(audio_graph<xmodule*>& graph)
{
    return new cjfilter_module(graph);
};

std::string module_cjfilter__get_name()
{
    return "cjfilter";
};
