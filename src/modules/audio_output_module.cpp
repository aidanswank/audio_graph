#include "audio_output_module.h"

void audio_output_module::process()
{
//        std::cout <<  "id " << id << " final output" << std::endl;
//        xmodule::audio.clear();
    
//        memset(xmodule::audio.data(), 0.0f, sizeof(float)*256);
    zero_audio(xmodule::output_audio,256);
    
    for(int i = 0; i < input_ids.size(); i++)
    {
        for(int j = 0; j < input_ids[i].size(); j++)
        {
//            print(i,j,"id",input_ids[i][j]);
            if(input_ids[i][j] != -1)
            {
                xmodule *mod = graph.xmodules[ input_ids[i][j] ];
                // TODO CHANGE FIXED BUFFER
                for(int i = 0; i < 256; i++)
                {
                    //                xmodule::audio[i] += mod->audio[i];
                    
                    double exp_adjusted_slider = pow(output_gain,2); // make slider exponentional because volume intensity response of our ear is logarithmic
                    
//                    print(exp_adjusted_slider);
                    
                    xmodule::output_audio[0][i] += mod->output_audio[0][i] * exp_adjusted_slider;
                    xmodule::output_audio[1][i] += mod->output_audio[1][i] * exp_adjusted_slider;
                }
                //            mod->audio.clear();
                //            std::cout <<  "summing " << i << std::endl;
            }
        }
    }

};

xmodule* module_audio_output__create(audio_graph<xmodule*>& graph)
{
    return new audio_output_module(graph);
};

std::string module_audio_output__get_name()
{
    return "audio output";
};
