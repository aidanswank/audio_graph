#include "osc_module.h"

osc_module::osc_module(audio_graph<xmodule*>& graph) : xmodule(graph)
{
    blep = new PolyBLEP(44100, PolyBLEP::SINE, 440.0f);
    name = module_osc__get_name();
    config(0,1);
};


void osc_module::process() {

    float *audio_output_left = xmodule::output_audio[0].data();
    float *audio_output_right = xmodule::output_audio[1].data();
    
    blep->setFrequency(freq);

    for (unsigned long i = 0; i < 256; ++i) {
        float samp = blep->getAndInc();
//        print("samp",samp);
        audio_output_left[i] = samp;
        audio_output_right[i] = samp;
    }
    
}

void osc_module::show() {
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
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

xmodule* module_osc__create(audio_graph<xmodule*>& graph)
{
    return new osc_module(graph);
};

std::string module_osc__get_name()
{
    return "polybleep osc";
};
