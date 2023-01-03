#include "xmodule.h"

struct audio_output_module : xmodule {
    
    audio_output_module(int id) : xmodule(id){}
    
    void show() override {};
    void poll() override {};
    
    void process(std::vector<xmodule*>& modules) override;
};
