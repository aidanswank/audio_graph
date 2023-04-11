#include "xmodule.h"
#include "vendor/CjFilter.h"

struct cjfilter_module : xmodule {
    float cutoff;
    float resonance;
    CjFilter filter[2];
    float filterOut[4];
        
    cjfilter_module(audio_graph<xmodule*>& graph, ImVec2 click_pos);
    void process() override;
    void show() override;
    void save_state(nlohmann::json& object) override;
    void load_state(nlohmann::json& object) override;
//    void save_state(nlohmann::json& object)
//    {
//        object["cutoff"] = cutoff;
//        object["resonance"] = resonance;
//    };
    
};

xmodule* module_cjfilter__create(audio_graph<xmodule*>& graph, ImVec2 click_pos);
std::string module_cjfilter__get_name();
