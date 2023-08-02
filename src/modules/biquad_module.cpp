//#pragma once;
#include "xmodule.h"
#include "biquad_module.h"

biquad_module::biquad_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(2,1);
    name = module_biquad__get_name();
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    cutoff = 1500.0;
    resonance = 0.707;
    current_mode = 0;
    
    filter = new Biquad();    // create a Biquad, lpFilter;
    filter->setBiquad(bq_type_lowpass, cutoff / 44100.0, resonance, 0);
};

void biquad_module::process()
{
    
    if(input_ids[0])
    {
        
        
        xmodule* input_module = (xmodule*)graph.xmodules[ input_ids[0] ];
        STEREO_AUDIO new_input_audio = input_module->output_audio;

        for(int i = 0; i < 256; i++)
        {
            if(input_ids[1])
            {
                STEREO_AUDIO input_cutoff_module = ((xmodule*)graph.xmodules[ input_ids[1] ])->output_audio;
//                print("cutoff plugged in", input_cutoff_module[0][0]);
                float left_sig = input_cutoff_module[0][i];
                cutoff = left_sig * 16000.0;
            }
            filter->setBiquad(current_mode, cutoff / 44100.0, resonance, 0);
            float new_samp_l = filter->process(new_input_audio[0][i]);
//            print(new_samp_l);
            float new_samp_r = new_samp_l;
            output_audio[0][i] = new_samp_l;
            output_audio[1][i] = new_samp_r;
        }
    }
};

void biquad_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    ImNodes::BeginInputAttribute( input_attrs[ 1 ] );
    ImGui::Text("cutoff");
    ImNodes::EndInputAttribute();
    
//    ImNodes::BeginInputAttribute( input_attrs[ 2 ] );
//    ImGui::Text("resonance");
//    ImNodes::EndInputAttribute();
    
    ImGui::PushItemWidth(120);

    const char* items[] = { "lowpass", "highpass", "bandpass", "notch", "peak", "lowshelf", "highshelf" };
    
    if(ImGui::Combo("##combo", &current_mode, items,
                 IM_ARRAYSIZE(items)))
    {
        print("set filter mode ",current_mode);
    }
    
    ImGui::SliderFloat("cutoff", &cutoff, 30.0f, 16000);
    ImGui::SliderFloat("resonance", &resonance, 0.1f, 1.2f);

    ImGui::PopItemWidth();

    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};

void biquad_module::save_state(nlohmann::json& object)
{
    object["current_mode"] = current_mode;
    object["cutoff"] = cutoff;
    object["resonance"] = resonance;
};

void biquad_module::load_state(nlohmann::json& object)
{
    current_mode = object["current_mode"];
    cutoff = object["cutoff"];
    resonance = object["resonance"];
};

xmodule* module_biquad__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new biquad_module(graph, click_pos);
};

std::string module_biquad__get_name()
{
    return "biquad filter";
};


