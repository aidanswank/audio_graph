#include "osc_module.h"
#include "midi_in_module.h"

osc_module::osc_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    blep = new PolyBLEP(44100, PolyBLEP::SINE, 440.0f);
    name = module_osc__get_name();
        
//    float* secret_luv_uu = new float(0);
    input_void_ptr = (void*)new float(0);
    
    config(2,1);
    
    // register_param(current_patch[])
    
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
};


void osc_module::process() {

    float *audio_output_left = xmodule::output_audio[0].data();
    float *audio_output_right = xmodule::output_audio[1].data();
    xmodule* freq_input_mod = NULL;

    if(input_ids[0][0]!=-1)
    {
////        print("in 1");
//        xmodule* input = (xmodule*)graph.xmodules[ input_ids[0][0] ];
//        float* test_in = (float*)input->output_void_ptr;
//        if(test_in==NULL)
//        {
//            print("CONNECTED INPUT IS NULL!");
//        } else {
//            print(*test_in);
////            freq = midi2freq(*test_in);
//            freq = *test_in;
//
////            blep->setFrequency(midi2freq(*test_in));
//        }
        
        freq_input_mod = (xmodule*)graph.xmodules[ input_ids[0][0] ];
//        freq_input_mod->output_audio[i][0]
        

    }
    
//    if(input_ids[1][0]!=-1)
//    {
//        print("in 2");
//    }
//
//    blep->setFrequency(freq);

//    if(freq_input_mod) {
////        print(output_audio[0][0]);
//        freq = freq_input_mod->output_audio[0][0];
//    }
//
    blep->setFrequency(freq);
    
    for (unsigned long i = 0; i < 256; ++i) {
        
        if(freq_input_mod) {
    //        print(output_audio[0][0]);
            freq = freq_input_mod->output_audio[0][i];
        }
        
        blep->setFrequency(freq);
        
        float samp = blep->getAndInc();
        
        audio_output_left[i] = samp;
        audio_output_right[i] = samp;
    }
    
}

void osc_module::show() {
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[0] );
    ImGui::Text("freq in");
    ImNodes::EndInputAttribute();
    
//    ImNodes::BeginInputAttribute( input_attrs[1] );
//    ImGui::Text("not real");
//    ImNodes::EndInputAttribute();
    
    ImGui::PushItemWidth(100.0f);
    ImGui::Checkbox("LFO", &isLFO);
    if(isLFO)
    {
        ImGui::SliderFloat("freq", &freq, 0.0f, 30.0f);
    } else {
        ImGui::SliderFloat("freq", &freq, 0.0f, 1000.0f);
    }
    
    const char* items[] = { "SINE", "COSINE", "TRIANGLE", "SQUARE", "RECTANGLE", "SAWTOOTH", "RAMP", "MODIFIED_TRIANGLE", "MODIFIED_SQUARE", "HALF_WAVE_RECTIFIED_SINE", "FULL_WAVE_RECTIFIED_SINE", "TRIANGULAR_PULSE", "TRAPEZOID_FIXED", "TRAPEZOID_VARIABLE" };
    
    if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(items[n], is_selected))
            {
                current_item = items[n];
                blep->setWaveform((PolyBLEP::Waveform)n);
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
        }
        ImGui::EndCombo();
    }
    
    
    ImGui::PopItemWidth();
    
    ImNodes::BeginOutputAttribute( output_attrs[0] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};

xmodule* module_osc__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new osc_module(graph, click_pos);
};

std::string module_osc__get_name()
{
    return "oscillator";
};
