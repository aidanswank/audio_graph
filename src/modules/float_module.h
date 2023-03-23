#include "xmodule.h"

struct float_module : xmodule {
    float data;
    float_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
};

xmodule* module_float__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_float__get_name();
