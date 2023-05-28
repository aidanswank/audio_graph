//#pragma once;
#include "xmodule.h"
#include "downsampler_module.h"

downsampler_module::downsampler_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_downsampler__get_name();
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    
    rate_adjustment = 0.25;
    output_sample_rate = sample_rate * rate_adjustment;
};

void downsampler_module::process()
{
    zero_audio(xmodule::output_audio,256);

//    float *inout_audio_left_ptr = xmodule::input_audio[0].data();
//    float *inout_audio_right_ptr = xmodule::input_audio[1].data();

//    output_sample_rate = sample_rate * rate_adjustment;

    if(input_ids[0][0]!=-1)
    {
        xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[0][0] ];
        
        for (int i = 0; i < 256; i += (int)(1 / rate_adjustment))
        {
            //        downsampledBlock.push_back(block[j]);
            output_audio[0][i] = midi_in_module->output_audio[0][i];
            output_audio[1][i] = midi_in_module->output_audio[0][i];
        }
    }
    
//    // loop through audio buffer, processing blocks of SAMPLE_SIZE at a time
//    for (int i = 0; i < 256; i += sample_size)
//    {
////      vector<double> block(audioBuffer.begin() + i, audioBuffer.begin() + i + sample_size);
//
//      // perform downsampling by skipping samples
//      std::vector<double> downsampled_block;
//
//      for (int j = 0; j < 256; j += (int)(1 / rate_adjustment))
//      {
////        downsampledBlock.push_back(block[j]);
//      }
//
//      // append downsampled block to output buffer
////      audioBuffer.insert(audioBuffer.end(), downsampledBlock.begin(), downsampledBlock.end());
//    }

    
};

void downsampler_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    ImGui::PushItemWidth(100.0f);
    
//        char label[16];
//        sprintf(label, "##slider_%d", i);
    ImGui::SliderFloat("rate", &rate_adjustment, 0.0f, 1.0f);
    
    ImGui::PopItemWidth();
    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};

void downsampler_module::save_state(nlohmann::json& object)
{
    
};

void downsampler_module::load_state(nlohmann::json& object)
{
    
};

xmodule* module_downsampler__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new downsampler_module(graph, click_pos);
};

std::string module_downsampler__get_name()
{
    return "downsampler";
};


