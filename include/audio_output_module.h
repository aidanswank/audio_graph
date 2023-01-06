#include "xmodule.h"

struct audio_output_module : xmodule {
    
    audio_output_module(int id, audio_graph<xmodule*>& graph) : xmodule(id, graph){
        name = "output";
    }
    
    void show() override {
        ImNodes::BeginNode(xmodule::id);

        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute( id );
        ImGui::Text("input");
        ImNodes::EndInputAttribute();

        ImNodes::EndNode();        
    };
    void poll() override {};
    
    void process() override;
};
