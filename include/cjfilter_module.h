#include "xmodule.h"
#include "vendor/CjFilter.h"

struct cjfilter_module : xmodule {
    float cutoff;
    float resonance;
    CjFilter filter[2];
    float filterOut[4];
        
    cjfilter_module(audio_graph<xmodule*>& graph);
    void process() override;
    void show() override;
    
};

xmodule* module_cjfilter__create(audio_graph<xmodule*>& graph);
std::string module_cjfilter__get_name();
