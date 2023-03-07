#include "xmodule.h"

struct float_module : xmodule {
    float data;
    float_module(audio_graph<xmodule*>& graph);
    void process() override;
    void show() override;
};

xmodule* module_float__create(audio_graph<xmodule*>& graph);
std::string module_float__get_name();
