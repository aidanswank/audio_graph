#include "xmodule.h"
#include "EasyVst.h"
#include "SDL2/SDL.h"
#include "vprint.h"

struct vst3_midi_instrument : xmodule {
    
    EasyVst vst;
    uint64_t continuousSamples = 0;
    SDL_Event* event;
    
    enum input_id { MIDI_IN };

    vst3_midi_instrument(int id, SDL_Event* p_event);
    void process(std::vector<xmodule*>& modules) override;
    void show() override {
//        print("looop");
    };
    void poll() override {
        vst.processSdlEvent(*event);
    };
};
