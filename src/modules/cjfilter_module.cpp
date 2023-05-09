#include "cjfilter_module.h"

cjfilter_module::cjfilter_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
//    name = "cjfilter";
//    graph.push_unique_str(name);
    name = module_cjfilter__get_name();
    config(2,1);
    cutoff = 0.1;
    resonance = 0.0;
    ImNodes::SetNodeScreenSpacePos(id, click_pos);

//    input_void_ptr = new float[256](); // some mono signal to control cutoff
}

void cjfilter_module::process()
{
//    if(input_ids.size()>0)
//    {
////        print("true");
    
//    for(int i = 0; i < input_ids.size(); i++)
//    {
//        print(i, "ii", input_ids[i][0]);
//    }
        if(input_ids[0][0]!=-1)
        {
//            print("?????");
            xmodule* input_module = (xmodule*)graph.xmodules[ input_ids[0][0] ];
            
            STEREO_AUDIO input_audio = input_module->output_audio;
            
            for(int i = 0; i < 256; ++i)
            {
                if(input_ids[1][0]!=-1) // check if input 2 plugged in
                {
                    xmodule* input2_test = (xmodule*)graph.xmodules[ input_ids[1][0] ];
                    float sig = input2_test->output_audio[0][i];
                    sig = (sig + 1.0) / 2.0; // scale -1.0 to 1.0 -> 0.0 to 1.0
//                    sig = pow(sig,3);
                    cutoff = sig;
                }
                filter[0].doFilter(input_audio[0][i], cutoff, resonance);
                filter[1].doFilter(input_audio[1][i], cutoff, resonance);
                xmodule::output_audio[0][i] = filter[0].filterOut[0];
                xmodule::output_audio[1][i] = filter[1].filterOut[0]; //todo add another cjfilter class n do stereo
            }
        }
//    }
}

void cjfilter_module::show()
{
    ImNodes::BeginNode(xmodule::id);
    
    ImNodes::BeginNodeTitleBar();
    ImGui::Text( "%s (%i)", xmodule::name.c_str(), id );
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute( input_attrs[0] );
    ImGui::Text("audio input");
    ImNodes::EndInputAttribute();
    
    ImNodes::BeginInputAttribute( input_attrs[1] );
    ImGui::Text("cutoff");
    ImNodes::EndInputAttribute();
    
    ImGui::PushItemWidth(100.0f);
    ImGui::SliderFloat("cutoff", &cutoff, 0.0f, 1.0f);
    ImGui::SliderFloat("resonance", &resonance, 0.0f, 1.0f);
    ImGui::PopItemWidth();
    
    ImNodes::BeginOutputAttribute( output_attrs[0] );
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    
    ImNodes::EndNode();
};

void cjfilter_module::save_state(nlohmann::json& object)
{
    object["cutoff"] = cutoff;
    object["resonance"] = resonance;
    print(object);
};

void cjfilter_module::load_state(nlohmann::json& object)
{
//    cutoff = object["cutoff"];
    check_and_load(object, "cutoff", &cutoff);
//    resonance = object["resonance"];
    check_and_load(object, "resonance", &resonance);
//    print("loaded",object);
};

xmodule* module_cjfilter__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new cjfilter_module(graph, click_pos);
};

std::string module_cjfilter__get_name()
{
    return "cjfilter";
};
