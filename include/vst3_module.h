#include "xmodule.h"
#include "EasyVst.h"
#include "SDL2/SDL.h"
#include "vprint.h"
#include "imnodes.h"


struct vst3_midi_instrument : xmodule {
    
    EasyVst vst;
    uint64_t continuousSamples = 0;
    SDL_Event* event;
    
    enum input_id { MIDI_IN };

    vst3_midi_instrument(int id, SDL_Event* p_event, audio_graph<xmodule*>& graph);
    void process() override;
    void show() override {
        ImNodes::BeginNode(xmodule::id);
        
        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute( xmodule::id );
        ImGui::Text("input");
        ImNodes::EndInputAttribute();
        
        if(ImGui::Button("open"))
        {
            print("open vst", id);
            /* made everything in easyvst public to access window */
            SDL_ShowWindow(vst._window);
            SDL_RaiseWindow(vst._window);
        }
        
//        ImNodes::BeginOutputAttribute( xmodule::id << 8 );
//        ImGui::Text("output");
//        ImNodes::EndOutputAttribute();
        
        ImNodes::EndNode();
    };
    
    // modified easyvst so it doesnt destroy window it just hides it! beware!
    void poll() override {
        vst.processSdlEvent(*event);
    };
};
