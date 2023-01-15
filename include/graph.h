#include <vector>
//#include "xmodule.h"
#include "SDL.h"
#include <map>

// I made this a template because I wanted to be able to pass in graph inside xmodule class
// but I couldnt do that without creating circular dependencies
template <typename T>
struct audio_graph
{
    std::vector<int> visited;
    std::vector<int> process_order;
    std::vector<T> xmodules;
    std::vector<std::pair<int, int>> links;
    unsigned int attr_counter = 0;
    unsigned int id_counter = 0;
    std::vector<std::string> module_names;
    int root_id = -1;
    std::map<int, int> attr2id;
    
    SDL_Event *event;
    
//    void push_unique_str(std::string str)
//    {
//        if (std::find(module_names.begin(), module_names.end(), str) == module_names.end())
//        {
//          // Element in vector.
//            module_names.push_back(str);
//        }
//    }
    
//    int attr2id(int io_attr)
//    {
////        bool found;
//        for(int i = 0; i < xmodules.size(); i++)
//        {
//            for(int j = 0; j < xmodules[i]->input_attrs.size(); j++)
//            {
//                if(xmodules[i]->input_attrs[j]==io_attr)
//                {
//                    return xmodules[i]->id;
//                }
//            }
//            for(int j = 0; j < xmodules[i]->output_attrs.size(); j++)
//            {
//                if(xmodules[i]->output_attrs[j]==io_attr)
//                {
//                    return xmodules[i]->id;
//                }
//            }
//        }
//        return -1;
//    }
    
    void clear()
    {
        visited.clear();
        process_order.clear();
    };
    
    // need to run clear() before search
    void DFS(int rootId)
    {
        if (find(visited.begin(), visited.end(), rootId) != visited.end())
            return;                // Return if the node has already been visited
        visited.push_back(rootId); // Mark the node as visited
        // process the audio signal for the input xmodules of the current node
        for (int input_id : xmodules[rootId]->input_ids)
        {
            DFS(input_id);
        }
//        print("processing",rootId,"...");
        xmodules[rootId]->process(); // process the audio signal for the current node
//        process_order.push_back(rootId);
        // process the audio signal for the output xmodules of the current node
        for (int output_id : xmodules[rootId]->output_ids)
        {
            DFS(output_id); // Recursively process the audio signal for the output nodes
        }
    };
};
