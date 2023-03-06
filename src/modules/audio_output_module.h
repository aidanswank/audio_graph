#include "xmodule.h"

xmodule* module_audio_output__create(audio_graph<xmodule*>& graph);
std::string module_audio_output__get_name();


struct audio_output_module : xmodule {
    
    float output_gain;
    
    audio_output_module(audio_graph<xmodule*>& graph) : xmodule(graph){
        name = module_audio_output__get_name();
        output_gain = 0.0f;
        config(1,0);
    }
    
    void show() override {
        ImNodes::BeginNode(xmodule::id);

        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();
        
        ImGui::PushItemWidth(100.0f);
        ImGui::SliderFloat("gain", &output_gain, 0.0f, 1.0f);
        ImGui::PopItemWidth();

        ImNodes::BeginInputAttribute( input_attrs[0] );
        ImGui::Text("input");
        ImNodes::EndInputAttribute();

        ImNodes::EndNode();
    };
    
    void process() override;
};
