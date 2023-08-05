#include "user_interface.h"
#include "audio_callback.h"

void audio_settings_gui(audio_interface* interface)
{
    static bool audio_settings_open = true;
    if(audio_settings_open)
    {
        
        ImGui::Begin("audio settings", &audio_settings_open);
        
        static const char* current_item = NULL;
        
        if (ImGui::BeginCombo("input devices", current_item)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < interface->device_infos.size(); n++)
            {
                if(interface->device_infos[n].maxInputChannels>0)
                {
                    bool is_selected = (current_item == interface->device_infos[n].name); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(interface->device_infos[n].name, is_selected))
                    {
                        current_item = interface->device_infos[n].name;
                        interface->set_param(true, n);
                        
                        // dont set mic and then retry callback
                        // if i select speaker then mic with this uncommented i get glitchy audioß
                        //                    interface->try_params();
                        //                    interface->turn_on(audio_callback);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
            }
            ImGui::EndCombo();
        }
        
        static const char* current_item2 = NULL;
        if (ImGui::BeginCombo("outout devices", current_item2))
        {
            for (int n = 0; n < interface->device_infos.size(); n++)
            {
                if(interface->device_infos[n].maxOutputChannels>0)
                {
                    bool is_selected = (current_item2 == interface->device_infos[n].name);
                    if (ImGui::Selectable(interface->device_infos[n].name, is_selected))
                    {
                        current_item2 = interface->device_infos[n].name;
                        print("dev id", n);
                        interface->set_param(false, n);
                        interface->try_params();
                        interface->turn_on(audio_callback);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::End();
    }
}

void link_module(int start_attr, int end_attr, audio_graph<xmodule*>* graph)
{
        
    print("start", start_attr, "end", end_attr);
    print("arr2id map start", graph->attr2id[start_attr], "end", graph->attr2id[end_attr]);
//    print("end attr2inslots", graph->attr2inslot[end_attr]);


//    std::vector<int>& start_attr_vector  = graph->xmodules[ graph->attr2id[start_attr] ]->output_ids[ graph->attr2outslot[start_attr] ];
//
//    std::vector<int>& end_attr_vector = graph->xmodules[ graph->attr2id[end_attr] ]->input_ids[ graph->attr2inslot[end_attr] ];
//
//    // if there was nothing connected before (-1) put it in the first slot which is 0
//    // else if theres already a cable connected push it onto the vector (e.g the final audio output were multiple cables to be connected to same slot)
//
//    if(start_attr_vector[0]==-1)
//    {
//        start_attr_vector[0] = graph->attr2id[end_attr];
//    } else {
//        start_attr_vector.push_back(graph->attr2id[end_attr]);
//    }
//
//    if(end_attr_vector[0]==-1)
//    {
//        end_attr_vector[0] = graph->attr2id[start_attr];
//    } else {
//        end_attr_vector.push_back( graph->attr2id[start_attr] );
//    }
//
//    graph->xmodules[ graph->attr2id[start_attr] ]->add_output( graph->attr2id[end_attr] );
//
//    graph->xmodules[ graph->attr2id[end_attr] ]->add_input( graph->attr2id[start_attr] );
    
    graph->xmodules[ graph->attr2id[start_attr] ]->add_output( graph->attr2id[end_attr], graph->attr2outslot[start_attr] );

    graph->xmodules[ graph->attr2id[end_attr] ]->add_input( graph->attr2id[start_attr], graph->attr2inslot[end_attr] );
    
//    print("slot", graph->attr2inslot[end_attr], graph->attr2inslot[start_attr], graph->attr2outslot[end_attr], graph->attr2outslot[start_attr] );
    
    graph->links.push_back(std::make_pair(start_attr, end_attr));
}

void remove_link(int edge_id, audio_graph<xmodule*>* graph)
{
    
    int start_attr = graph->links[ edge_id ].first;
    int end_attr = graph->links[ edge_id ].second;
        print("start_attr", start_attr, "end attr",end_attr);
        print(
              "start id",graph->xmodules[ graph->attr2id[start_attr] ]->id,
              "end id",graph->xmodules[ graph->attr2id[end_attr] ]->id
        );
    
    int x = graph->xmodules[ graph->attr2id[start_attr] ]->output_ids[ graph->attr2outslot[start_attr] ];

    int y = graph->xmodules[ graph->attr2id[end_attr] ]->input_ids[ graph->attr2inslot[end_attr] ];
    
    print("x",x,y);
    
    graph->xmodules[ graph->attr2id[start_attr] ]->output_ids.erase( graph->attr2outslot[start_attr] );

    graph->xmodules[ graph->attr2id[end_attr] ]->input_ids.erase( graph->attr2inslot[end_attr] );
        
//    // why did i write end_attr twice in attr2outslot and attr2inslot was that intentional or glitch??? i forgot how this works
////    std::vector<int>& start_attr_vector  = graph->xmodules[ graph->attr2id[start_attr] ]->output_ids[ graph->attr2outslot[end_attr] ];
//
//    std::vector<int>& start_attr_vector  = graph->xmodules[ graph->attr2id[start_attr] ]->output_ids[ graph->attr2outslot[start_attr] ];
//    std::vector<int>& end_attr_vector    = graph->xmodules[ graph->attr2id[end_attr] ]->input_ids[ graph->attr2inslot[end_attr] ];
//
////
////    start_attr_vector[ graph->attr2inslot[start] ] = -1;
////    end_attr_vector[ graph->attr2inslot[end_attr] ] = -1;
//
//
////    print("!!!!!!!!!!!",graph->attr2inslot[start_attr],graph->attr2outslot[end_attr]);
//
//    // fill input and output slots with -1
//    for(int i = 0; i < start_attr_vector.size(); i++)
//    {
//        start_attr_vector[i] = -1;
//    }
//
//    for(int i = 0; i < end_attr_vector.size(); i++)
//    {
//        end_attr_vector[i] = -1;
//    }
//
////    int start = graph->links[edge_id].first;
////    int end = graph->links[edge_id].second;
////    int index = 0;
////    for (auto& element : current_patch["links"])
////    {
//////        link_module(element["start"], element["end"], graph);
////        if(element["start"]==start&&element["end"]==end)
////        {
////            print("found link in patch");
////            current_patch["links"].erase(current_patch["links"].begin())
////        }
////        index++;
////    }
//
    current_patch["links"].erase(current_patch["links"].begin() + edge_id);
    print("erased!",edge_id,"current_patch",current_patch);

    // BREAKS WHEN DELETETING MULTIPLE NODES GOTTA FIX
    graph->links.erase(graph->links.begin() + edge_id);

}
