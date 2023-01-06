//#include "graph.h"
//
//template <typename T>
//void audio_graph<T>::clear()
//{
//    visited.clear();
//    process_order.clear();
//}
//
//// need to run clear() before search
//template <typename T>
//void audio_graph<T>::DFS(int rootId)
//{
//    if (find(visited.begin(), visited.end(), rootId) != visited.end())
//        return;                // Return if the node has already been visited
//    visited.push_back(rootId); // Mark the node as visited
//    // process the audio signal for the input xmodules of the current node
//    for (int input_id : xmodules[rootId]->input_ids)
//    {
//        DFS(input_id);
//    }
////        print(rootId);
//    xmodules[rootId]->process(); // process the audio signal for the current node
//    process_order.push_back(rootId);
//    // process the audio signal for the output xmodules of the current node
//    for (int output_id : xmodules[rootId]->output_ids)
//    {
//        DFS(output_id); // Recursively process the audio signal for the output nodes
//    }
//}
