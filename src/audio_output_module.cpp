#include "audio_output_module.h"

void audio_output_module::process()
{
//        std::cout <<  "id " << id << " final output" << std::endl;
//        xmodule::audio.clear();
    
//        memset(xmodule::audio.data(), 0.0f, sizeof(float)*256);
    zero_audio(xmodule::output_audio,256);
    
    for(int i = 0; i < input_ids.size(); i++)
    {
        xmodule *mod = modules[ input_ids[i] ];
        // TODO CHANGE FIXED BUFFER
        for(int i = 0; i < 256; i++)
        {
//                xmodule::audio[i] += mod->audio[i];
            xmodule::output_audio[0][i] += mod->output_audio[0][i];
            xmodule::output_audio[1][i] += mod->output_audio[1][i];
        }
//            mod->audio.clear();
//            std::cout <<  "summing " << i << " audio size " << mod->audio.size() << std::endl;
    }

};
