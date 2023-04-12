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

void check_and_set_zero(json& obj, const std::string& key) {
    // Check if the key exists in the object
    if (obj.find(key) == obj.end()) {
        // Key doesn't exist, create it with a value of zero
        obj[key] = 0;
    } else {
        // Key exists, check if it is null
        if (obj[key].is_null()) {
            // Key is null, set it to zero
            obj[key] = 0;
        }
    }
}
