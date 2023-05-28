//#pragma once
#include "xmodule.h"
#include "polysampler_module.h"

polysampler_module::polysampler_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    name = module_polysampler__get_name();
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
    
    open_file_dialog = new ImGui::FileBrowser();
    open_file_dialog->SetTitle("open file");
    open_file_dialog->SetTypeFilters({ ".WAV",".wav",".mp3",".ogg"});
    
    sampler = new polysampler(16);
};

void polysampler_module::process()
{
    zero_audio(xmodule::output_audio,256);
    float *audio_output_left_ptr = xmodule::output_audio[0].data();
    float *audio_output_right_ptr = xmodule::output_audio[1].data();
    
    if(input_ids[0][0]!=-1)
    {
        xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[0][0] ];
        
        for(int i = 0; i < midi_in_module->input_notes.size(); i++)
        {
            midi_note_message note = midi_in_module->input_notes[i];
//            print("incoming notes..", note.note_num, note.velocity, note.is_note_on, "+bend", note.pitch_bend_a, note.pitch_bend_b);
            sampler->send_message(note);
        }
        
        sampler->generate_samples(audio_output_left_ptr, 256);
        memcpy(audio_output_right_ptr, audio_output_left_ptr, 256*sizeof(float));
    }
};

void polysampler_module::show(){
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[ 0 ] );
    ImGui::Text("input");
    ImNodes::EndInputAttribute();
    
    
    //    const char* waveform_names[] = {
    //        "SINE", "COSINE", "TRIANGLE", "SQUARE", "RECTANGLE", "SAWTOOTH", "RAMP",
    //        "MODIFIED_TRIANGLE", "MODIFIED_SQUARE", "HALF_WAVE_RECTIFIED_SINE",
    //        "FULL_WAVE_RECTIFIED_SINE", "TRIANGULAR_PULSE", "TRAPEZOID_FIXED",
    //        "TRAPEZOID_VARIABLE"
    //    };
    
    //    static int current_waveform = 0;
    ImGui::PushItemWidth(100.0f);
    
    
    if(ImGui::Button("FUCK"))
    {
        open_file_dialog->Open();
    }
//    if(ImGui::Combo("Waveform", &current_waveform, waveform_names,
//                 IM_ARRAYSIZE(waveform_names)))
//    {
//        print("yo!",current_waveform);
//        for(int i = 0; i < synth->voices.size(); i++)
//        {
//            synth->voices[i].synth->setWaveform((PolyBLEP::Waveform(current_waveform)));            
//        }
//    }

    ImGui::PopItemWidth();
//    Waveform selected_waveform = static_cast<Waveform>(current_waveform);
    
    
    ImNodes::BeginOutputAttribute( output_attrs[ 0 ] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
    
    open_file_dialog->Display();

    if(open_file_dialog->HasSelected())
    {
        std::cout << "Selected filename" << open_file_dialog->GetSelected().string() << std::endl;
        sampler->load_sample(open_file_dialog->GetSelected().string());
        open_file_dialog->ClearSelected();
//        open_file_dialog->ClearSelected();
    }
};

void polysampler_module::save_state(nlohmann::json& object)
{
//    object["current_waveform"] = current_waveform;
//    print(object);
    object["filepath"] = sampler->filepath;
    
};

void polysampler_module::load_state(nlohmann::json& object)
{
//    check_and_load(object, "current_waveform", &current_waveform);
//    current_waveform = object["current_waveform"];
//    print("polysynth",object,current_waveform);
//    sampler->set_waveform(current_waveform);
//    check_and_load(object, "resonance", &resonance);
//    filepath = object["filepath"];
    
    sampler->filepath = object["filepath"];
    sampler->load_sample(sampler->filepath);
};

xmodule* module_polysampler__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new polysampler_module(graph, click_pos);
};

std::string module_polysampler__get_name()
{
    return "polysampler";
};


