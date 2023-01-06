#include "vst3_module.h"

vst3_midi_instrument::vst3_midi_instrument(int id, SDL_Event* p_event, std::vector<xmodule*>& modules) : xmodule(id,modules)
{
    name = "vst midi instrument";
    event = p_event;
    // Library/Audio/Plug-Ins/VST3/Surge XT.vst3
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
    
     if (!vst.createView()) {
           std::cerr << "Failed to create VST view" << std::endl;
        //    return 1;
     }

};

void vst3_midi_instrument::process()
{
    
    xmodule *midi_in_module = (xmodule*)xmodule::modules[ input_ids[MIDI_IN] ];
    
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
    for(int i = 0; i < midi_in_module->notes.size(); i++)
    {
        MidiNoteMessage note = midi_in_module->notes[i];
        
        Steinberg::Vst::Event evt = {};
        evt.busIndex = 0;
        evt.sampleOffset = 0;
        evt.ppqPosition = currentBeat;
        evt.flags = Steinberg::Vst::Event::EventFlags::kIsLive;
        if (note.isNoteOn) {
            //            print("note on");
            evt.type = Steinberg::Vst::Event::EventTypes::kNoteOnEvent;
            evt.noteOn.channel = 0;
            evt.noteOn.pitch = note.noteNum;
            evt.noteOn.tuning = 0.0f;
            evt.noteOn.velocity = note.velocity;
            evt.noteOn.length = 0;
            evt.noteOn.noteId = -1;
        } else {
            //            print("note off");
            evt.type = Steinberg::Vst::Event::EventTypes::kNoteOffEvent;
            evt.noteOff.channel = 0;
            evt.noteOff.pitch = note.noteNum;
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
    
    
//    float *audio_output = xmodule::audio.data();
//    zero_audio(xmodule::audio,256);
    float *audio_output_left = xmodule::output_audio[0].data();
    float *audio_output_right = xmodule::output_audio[1].data();
//
    for (unsigned long i = 0; i < 256; ++i) {
        audio_output_left[i] = left[i];
        audio_output_right[i] = right[i];
    }
    
}


