#include "xmodule.h"

xmodule* module_audio_output__create(audio_graph<xmodule*>& graph);
std::string module_audio_output__get_name();

struct audio_output_module : xmodule {
    
    audio_output_module(audio_graph<xmodule*>& graph) : xmodule(graph){
        name = module_audio_output__get_name();
        config(1,0);
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
    
    void process() override;
};
