#include "xmodule.h"
#include "vendor/CjFilter.h"

struct cjfilter_module : xmodule {
    float cutoff;
    float resonance;
    CjFilter filter[2];
    float filterOut[4];
    
    cjfilter_module(int id);
    void process(std::vector<xmodule*>& modules) override;
    void show() override {};
    void poll() override {};
};
