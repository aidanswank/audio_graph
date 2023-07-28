//#pragma once;
#include "xmodule.h"
#include "tube_module.h"

tube_module::tube_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    gain = 1.0;
    shape1 = 0.1;
    shape2 = 800.0;
    name = module_tube__get_name();
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
};

double tanh_distortion(double input, double gain, double shape1, double shape2)
{
    double output = tanh(input * gain);
    output *= 1.0 + shape1 * exp(-output * output / shape2);
    return output;
}

void tube_module::process()
{
    if(input_ids[0]!=-1)
    {
        xmodule* input_module = (xmodule*)graph.xmodules[ input_ids[0] ];
        STEREO_AUDIO new_input_audio = input_module->output_audio;
        for(int i = 0; i < 256; i++)
        {
            float new_samp_l = tanh_distortion(new_input_audio[0][i],gain,shape1,shape2);
            float new_samp_r = tanh_distortion(new_input_audio[1][i],gain,shape1,shape2);
            
            output_audio[0][i] = new_samp_l;
            output_audio[1][i] = new_samp_r;
        }
    }
    
};

void tube_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
//    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    ImGui::PushItemWidth(150.0f);

    
    // Initialize data for the plot
//    static std::vector<float> data = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
//    float x_min = 0.0f;
//    float x_max = (float)data.size() - 1.0f;
    
    ImGui::SliderFloat("shape1", &shape1, 0.1f, 1.0f);
    ImGui::SliderFloat("shape2", &shape2, 10.f, 1000.0f);
    ImGui::SliderFloat("gain", &gain, 0.f, 10.0);

    float y_min = -1.0f;
    float y_max = 1.0f;
    
    ImGui::PlotLines("##osc", &output_audio[0][0], (int)output_audio[0].size(), 0, NULL, y_min, y_max, ImVec2(0, 0), 4);
    
    ImGui::PopItemWidth();

    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
//    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};

void tube_module::save_state(nlohmann::json& object)
{
    object["gain"] = gain;
    object["shape1"] = shape1;
    object["shape2"] = shape2;
};

void tube_module::load_state(nlohmann::json& object)
{
    gain = object["gain"];
    shape1 = object["shape1"];
    shape2 = object["shape2"];
};

xmodule* module_tube__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new tube_module(graph, click_pos);
};

std::string module_tube__get_name()
{
    return "tube";
};


