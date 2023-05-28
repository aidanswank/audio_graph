#include "xmodule.h"

void zero_audio(STEREO_AUDIO &audio, int len)
{
    audio.clear();
    std::vector<float> channel_1(len, 0.0f);
    std::vector<float> channel_2(len, 0.0f);
    audio.push_back(channel_1);
    audio.push_back(channel_2);
    
//    for(int i = 0; i < 2; i++)
//    {
//        memset(audio[i].data(), 0.0f, sizeof(float)*len);
//    }
}

void check_and_load(json& obj, std::string key, void* data) {
    if (obj.contains(key))
    {
        data = &obj[key];
        print(key," exist");
//        print(data,obj[key]);
    } else {
        print("param '",key,"' not found in file! setting to zero");
        data = 0;
    }
    
}
