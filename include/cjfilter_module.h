#include "xmodule.h"
#include "vendor/CjFilter.h"

struct cjfilter_module : xmodule {
    float cutoff;
    float resonance;
    CjFilter filter[2];
    float filterOut[4];
    
    cjfilter_module(int p_id, audio_graph<xmodule*>& graph);
    void process() override;
    void show() override {
        ImNodes::BeginNode(xmodule::id);

        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute( id );
        ImGui::Text("input");
        ImNodes::EndInputAttribute();

//        ImNodes::BeginOutputAttribute( id );
//        ImGui::Text("output");
//        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    };
    void poll() override {};
};
