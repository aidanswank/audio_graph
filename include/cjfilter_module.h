#include "xmodule.h"
#include "vendor/CjFilter.h"

struct cjfilter_module : xmodule {
    float cutoff;
    float resonance;
    CjFilter filter[2];
    float filterOut[4];
    
    cjfilter_module(audio_graph<xmodule*>& graph);
    void process() override;
    void show() override {
        ImNodes::BeginNode(xmodule::id);

        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute( input_attrs[0] );
        ImGui::Text("input");
        ImNodes::EndInputAttribute();
        
        ImGui::PushItemWidth(100.0f);
        ImGui::SliderFloat("cutoff", &cutoff, 0.0f, 1.0f);
        ImGui::SliderFloat("resonance", &resonance, 0.0f, 1.0f);
        ImGui::PopItemWidth();

        ImNodes::BeginOutputAttribute( output_attrs[0] );
        ImGui::Text("output");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    };
    void poll() override {};
};

xmodule* module_cjfilter__create(audio_graph<xmodule*>& graph);
std::string module_cjfilter__get_name();
