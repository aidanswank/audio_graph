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

    vst3_midi_instrument(int id, SDL_Event* p_event, std::vector<xmodule*>& modules);
    void process() override;
    void show() override {
//        print("looop");
//        const int hardcoded_node_id = 1;

        ImNodes::BeginNode(xmodule::id);
//        ImGui::Dummy(ImVec2(80.0f, 45.0f));
//        print(modules[ input_ids[0] ]->id);
        ImNodes::BeginNodeTitleBar();
        ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute( xmodule::id );
        ImGui::Text("input");
        ImNodes::EndInputAttribute();
        
        ImNodes::BeginOutputAttribute( modules[ output_ids[0] ]->id );
        ImGui::Text("output");
        ImNodes::EndOutputAttribute();
        
        ImNodes::EndNode();

    };
    void poll() override {
        vst.processSdlEvent(*event);
    };
};
