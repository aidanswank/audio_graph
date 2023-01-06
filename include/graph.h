#include <vector>
//#include "xmodule.h"

// I made this a template because I wanted to be able to pass in graph inside xmodule class
// but I couldnt do that without creating circular dependencies
template <typename T>
struct audio_graph
{
    std::vector<int> visited;
    std::vector<int> process_order;
    std::vector<T> xmodules;
    std::vector<std::pair<int, int>> links;
    
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
