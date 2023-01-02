#include "xmodule.h"
#include "EasyVst.h"

struct vst3_midi_instrument : xmodule {
    
    EasyVst vst;
    uint64_t continuousSamples = 0;
    
    vst3_midi_instrument(int id);
    void process(std::vector<xmodule*>& modules) override;
};


//EasyVst vst;
//module_data->vst = vst;
//module_data->audio_output = new float[256]();
/////Library/Audio/Plug-Ins/VST3/Surge XT.vst3
///////////////// vst setup TODO CLEAN !!!!
//if (!module_data->vst.init("/Library/Audio/Plug-Ins/VST3/Surge XT.vst3", 44100, 256, Steinberg::Vst::kSample32, true))
//{
//    std::cerr << "Failed to initialize VST" << std::endl;
//    // return 1;
//}
//
//int numEventInBuses = module_data->vst.numBuses(Steinberg::Vst::kEvent, Steinberg::Vst::kInput);
//int numAudioOutBuses = module_data->vst.numBuses(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput);
//if (numEventInBuses < 1 || numAudioOutBuses < 1) {
//        std::cerr << "Incorrect bus configuration" << std::endl;
//        // return 1;
//}
//
//const Steinberg::Vst::BusInfo *outBusInfo = module_data->vst.busInfo(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, 0);
//if (outBusInfo->channelCount != 2) {
//        std::cerr << "Invalid output channel configuration" << std::endl;
//        // return 1;
//}

//module_data->vst.setBusActive(Steinberg::Vst::kEvent, Steinberg::Vst::kInput, 0, true);
//module_data->vst.setBusActive(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, 1, true);
//module_data->vst.setProcessing(true);
//
//
// if (!module_data->vst.createView()) {
//       std::cerr << "Failed to create VST view" << std::endl;
//    //    return 1;
// }
