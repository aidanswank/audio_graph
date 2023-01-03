#include "audio_output_module.h"

void audio_output_module::process(std::vector<xmodule*>& modules)
{
//        std::cout <<  "id " << id << " final output" << std::endl;
//        xmodule::audio.clear();
    
//        memset(xmodule::audio.data(), 0.0f, sizeof(float)*256);
    zero_audio(xmodule::audio,256);
    
    for(int i = 0; i < input_ids.size(); i++)
    {
        xmodule *mod = modules[ input_ids[i] ];
        // TODO CHANGE FIXED BUFFER
        for(int i = 0; i < 256; i++)
        {
//                xmodule::audio[i] += mod->audio[i];
            xmodule::audio[0][i] += mod->audio[0][i];
            xmodule::audio[1][i] += mod->audio[1][i];
        }
//            mod->audio.clear();
//            std::cout <<  "summing " << i << " audio size " << mod->audio.size() << std::endl;
    }

};
