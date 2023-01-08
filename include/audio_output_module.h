#include "xmodule.h"

struct audio_output_module : xmodule {
    
    audio_output_module(audio_graph<xmodule*>& graph) : xmodule(graph){
        config("output",1,0);
    }
    
    void show() override {
        ImNodes::BeginNode(xmodule::id);

        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute( input_attrs[0] );
        ImGui::Text("input");
        ImNodes::EndInputAttribute();

        ImNodes::EndNode();
    };
    void poll() override {};
    
    void process() override;
};
