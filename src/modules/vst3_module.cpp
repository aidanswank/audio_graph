#include "vst3_module.h"
#include "public.sdk/source/vst/utility/stringconvert.h"

void vst3_midi_instrument::save_state(nlohmann::json &object)
{
    
};

void vst3_midi_instrument::load_state(nlohmann::json &object)
{
    
};

vst3_midi_instrument::vst3_midi_instrument(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    config(1,1);
    
//    event = p_event;
    
    event = graph.event;
    name = module_vst3_instrument__get_name();
    
//    float *input_mono_signal = new float[256]();
//    input_void_ptr = input_mono_signal;
    
    // Library/Audio/Plug-Ins/VST3/Surge XT.vst3
//    /Library/Audio/Plug-Ins/VST/Sitala.vst
    // vst setup TODO CLEAN !!!!
    if (!vst.init("/Library/Audio/Plug-Ins/VST3/Surge XT.vst3", 44100, 256, Steinberg::Vst::kSample32, true))
    {
        std::cerr << "Failed to initialize VST" << std::endl;
        // return 1;
    }
    
    int numEventInBuses = vst.numBuses(Steinberg::Vst::kEvent, Steinberg::Vst::kInput);
    int numAudioOutBuses = vst.numBuses(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput);
    if (numEventInBuses < 1 || numAudioOutBuses < 1) {
            std::cerr << "Incorrect bus configuration" << std::endl;
            // return 1;
    }
    
    const Steinberg::Vst::BusInfo *outBusInfo = vst.busInfo(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, 0);
    if (outBusInfo->channelCount != 2) {
            std::cerr << "Invalid output channel configuration" << std::endl;
            // return 1;
    }

    vst.setBusActive(Steinberg::Vst::kEvent, Steinberg::Vst::kInput, 0, true);
    vst.setBusActive(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, 1, true);
    vst.setProcessing(true);
    
    // get param names
    int num_params = vst._editController->getParameterCount();
    print("vst num params", num_params);
    for(int i = 0; i < vst._editController->getParameterCount(); i++)
    {
        Steinberg::Vst::ParameterInfo info;
        vst._editController->getParameterInfo(i, info);
        std::string title_str = VST3::StringConvert::convert(info.title); // whyyyy
        print(i, title_str);
    }
    
//    Steinberg::Vst::ParamValue myvalue = 0.0;
//    vst._editController->setParamNormalized(12, myvalue);
    
//    Steinberg::Vst::IParameterChanges* paramChanges = vst.parameterChanges(0, 0);
//    Steinberg::Vst::IParamValueQueue* paramQueue = paramChanges->addParameterData();
    
     if (!vst.createView()) {
           std::cerr << "Failed to create VST view" << std::endl;
        //    return 1;
     }
    SDL_HideWindow(vst._window);
    
    ImNodes::SetNodeScreenSpacePos(id, click_pos);

};

void vst3_midi_instrument::process()
{
//    xmodule::output_audio[0].clear();
//    xmodule::output_audio[1].clear();
    zero_audio(xmodule::output_audio,256);

    float *audio_output_left_ptr = xmodule::output_audio[0].data();
    float *audio_output_right_ptr = xmodule::output_audio[1].data();
    std::vector<midi_note_message> summed_input_notes;
    summed_input_notes.clear();
    
//    bool flag = false;
//
//    for(int i = 0; i < input_ids.size(); i++)
//    {
//        for(int j = 0; j < input_ids[i].size(); j++)
//        {
//            if(input_ids[i][j] != -1)
//            {
//
//                xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[i][j] ];
//
//                for(int k = 0; k < midi_in_module->input_notes.size(); k++)
//                {
//                    summed_input_notes.push_back(midi_in_module->input_notes[i]);
//                }
//
//                flag = true;
//            }
//        }
//    }
//
//    if(flag)
//    {
////        xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[0][0] ];
//
//        int SAMPLE_RATE = 44100;
//        int TEMPO = 120;
//
//        double currentBeat = continuousSamples / ((60.0 / TEMPO) * static_cast<double>(SAMPLE_RATE));
//
//        Steinberg::Vst::ProcessContext *processContext = vst.processContext();
//        processContext->state = Steinberg::Vst::ProcessContext::kPlaying;
//        processContext->sampleRate = SAMPLE_RATE;
//        processContext->projectTimeSamples = continuousSamples;
//        processContext->state |= Steinberg::Vst::ProcessContext::kTempoValid;
//        processContext->tempo = TEMPO;
//        processContext->state |= Steinberg::Vst::ProcessContext::kTimeSigValid;
//        processContext->timeSigNumerator = 4;
//        processContext->timeSigDenominator = 4;
//        processContext->state |= Steinberg::Vst::ProcessContext::kContTimeValid;
//        processContext->continousTimeSamples = continuousSamples;
//        processContext->state |= Steinberg::Vst::ProcessContext::kSystemTimeValid;
//        processContext->systemTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//        processContext->state |= Steinberg::Vst::ProcessContext::kProjectTimeMusicValid;
//        processContext->projectTimeMusic = currentBeat;
//        continuousSamples += 256;
//
//        Steinberg::Vst::EventList *eventList = vst.eventList(Steinberg::Vst::kInput, 0);
//
//        for(int i = 0; i < summed_input_notes.size(); i++)
//        {
//            MidiNoteMessage note = summed_input_notes[i];
//
//            Steinberg::Vst::Event evt = {};
//            evt.busIndex = 0;
//            evt.sampleOffset = 0;
//            evt.ppqPosition = currentBeat;
//            evt.flags = Steinberg::Vst::Event::EventFlags::kIsLive;
//    //            evt.flags = 1 << 14;
//            if (note.isNoteOn) {
//    //                            print("note on");
//                evt.type = Steinberg::Vst::Event::EventTypes::kNoteOnEvent;
//                evt.noteOn.channel = 0;
//                evt.noteOn.pitch = note.noteNum;
//                evt.noteOn.tuning = 0.0f;
//                evt.noteOn.velocity = note.velocity;
//                evt.noteOn.length = 0;
//                evt.noteOn.noteId = -1;
//            } else {
//    //                            print("note off");
//                evt.type = Steinberg::Vst::Event::EventTypes::kNoteOffEvent;
//                evt.noteOff.channel = 0;
//                evt.noteOff.pitch = note.noteNum;
//                evt.noteOff.tuning = 0.0f;
//                evt.noteOff.velocity = note.velocity;
//                evt.noteOff.noteId = -1;
//            }
//            eventList->addEvent(evt);
//        }
//
//        if (!vst.process(256)) {
//            std::cerr << "VST process() failed" << std::endl;
//            // return 1;
//        }
//
//        eventList->clear();
//
//        float *left = vst.channelBuffer32(Steinberg::Vst::kOutput, 0);
//        float *right = vst.channelBuffer32(Steinberg::Vst::kOutput, 1);
//
//        for (unsigned long i = 0; i < 256; ++i) {
//            audio_output_left_ptr[i] = left[i];
//            audio_output_right_ptr[i] = right[i];
//        }
//    } else {
//        print("nothing connected to vst?");
//        // pass silence;
//        for (unsigned long i = 0; i < 256; ++i) {
//            audio_output_left_ptr[i] = 0;
//            audio_output_right_ptr[i] = 0;
//        }
//    }
        
        
    if(input_ids[0]!=-1)
    {
        xmodule *midi_in_module = (xmodule*)xmodule::graph.xmodules[ input_ids[0] ];

        int SAMPLE_RATE = 44100;
        int TEMPO = 120;

        double currentBeat = continuousSamples / ((60.0 / TEMPO) * static_cast<double>(SAMPLE_RATE));

        Steinberg::Vst::ProcessContext *processContext = vst.processContext();
        processContext->state = Steinberg::Vst::ProcessContext::kPlaying;
        processContext->sampleRate = SAMPLE_RATE;
        processContext->projectTimeSamples = continuousSamples;
        processContext->state |= Steinberg::Vst::ProcessContext::kTempoValid;
        processContext->tempo = TEMPO;
        processContext->state |= Steinberg::Vst::ProcessContext::kTimeSigValid;
        processContext->timeSigNumerator = 4;
        processContext->timeSigDenominator = 4;
        processContext->state |= Steinberg::Vst::ProcessContext::kContTimeValid;
        processContext->continousTimeSamples = continuousSamples;
        processContext->state |= Steinberg::Vst::ProcessContext::kSystemTimeValid;
        processContext->systemTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        processContext->state |= Steinberg::Vst::ProcessContext::kProjectTimeMusicValid;
        processContext->projectTimeMusic = currentBeat;
        continuousSamples += 256;

        Steinberg::Vst::EventList *eventList = vst.eventList(Steinberg::Vst::kInput, 0);

        for(int i = 0; i < midi_in_module->input_notes.size(); i++)
        {
            midi_note_message note = midi_in_module->input_notes[i];

            Steinberg::Vst::Event evt = {};
            evt.busIndex = 0;
            evt.sampleOffset = 0;
            evt.ppqPosition = currentBeat;
            evt.flags = Steinberg::Vst::Event::EventFlags::kIsLive;
//            evt.flags = 1 << 14;
            if (note.is_note_on) {
//                            print("note on");
                evt.type = Steinberg::Vst::Event::EventTypes::kNoteOnEvent;
                evt.noteOn.channel = 0;
                evt.noteOn.pitch = note.note_num;
                evt.noteOn.tuning = 0.0f;
                evt.noteOn.velocity = note.velocity;
                evt.noteOn.length = 0;
                evt.noteOn.noteId = -1;
            } else {
//                            print("note off");
                evt.type = Steinberg::Vst::Event::EventTypes::kNoteOffEvent;
                evt.noteOff.channel = 0;
                evt.noteOff.pitch = note.note_num;
                evt.noteOff.tuning = 0.0f;
                evt.noteOff.velocity = note.velocity;
                evt.noteOff.noteId = -1;
            }
            eventList->addEvent(evt);
        }

        if (!vst.process(256)) {
            std::cerr << "VST process() failed" << std::endl;
            // return 1;
        }

        eventList->clear();

        float *left = vst.channelBuffer32(Steinberg::Vst::kOutput, 0);
        float *right = vst.channelBuffer32(Steinberg::Vst::kOutput, 1);

        for (unsigned long i = 0; i < 256; ++i) {
            audio_output_left_ptr[i] = left[i];
            audio_output_right_ptr[i] = right[i];
        }
    } else {
        print("nothing connected to vst?");
        // pass silence;
        for (unsigned long i = 0; i < 256; ++i) {
            audio_output_left_ptr[i] = 0;
            audio_output_right_ptr[i] = 0;
        }
    }
    
}

xmodule* module_vst3_instrument__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new vst3_midi_instrument(graph, click_pos);
};

std::string module_vst3_instrument__get_name(){
    return "vst instrument";
};
