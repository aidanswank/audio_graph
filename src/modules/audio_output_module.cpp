#include "audio_output_module.h"

void clip_audio(float& samp)
{
    if(samp>1.0)
    {
        samp=1;
    }
    if(samp<-1.0)
    {
        samp=-1;
    }
}

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
                    
                    clip_audio(xmodule::output_audio[0][i]);
                    clip_audio(xmodule::output_audio[1][i]);
                    
                    xmodule::output_audio[0][i] += mod->output_audio[0][i] * exp_adjusted_slider;
                    xmodule::output_audio[1][i] += mod->output_audio[1][i] * exp_adjusted_slider;
                    
//                    xmodule::output_audio[0][i] += rand()%1000000000000/1000000000000;
//                    xmodule::output_audio[1][i] += rand()%1000000000000/1000000000000;
                }
                //            mod->audio.clear();
                //            std::cout <<  "summing " << i << std::endl;
            }
        }
    }
    
    for(int i = 0; i < 256; i++)
    {
        if(isnan(xmodule::output_audio[0][i])||isnan(xmodule::output_audio[1][i]))
        {
            print("NaN detected!");
            xmodule::output_audio[0][i] += (float)(rand()%10000000000)/10000000000.0;
            xmodule::output_audio[1][i] += (float)(rand()%10000000000)/10000000000.0;
        }
    }

};

void audio_output_module::save_state(nlohmann::json &object)
{
    object["output_gain"] = output_gain;
    print("audio output state save", object);
};

void audio_output_module::load_state(nlohmann::json &object)
{
    print("audio output state load", object);
    check_and_load(object, "output_gain", &output_gain);
//    if (object.contains(std::string{ "output_gain" }))
//    {
//        output_gain = object["output_gain"];
//    }
//    does_param_exist(object, )
};

xmodule* module_audio_output__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new audio_output_module(graph, click_pos);
};

std::string module_audio_output__get_name()
{
    return "audio output";
};
