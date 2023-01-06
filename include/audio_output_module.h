#include "xmodule.h"

struct audio_output_module : xmodule {
    
    audio_output_module(int id, std::vector<xmodule*>& modules) : xmodule(id, modules){}
    
    void show() override {};
    void poll() override {};
    
    void process() override;
};
