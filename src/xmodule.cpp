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
