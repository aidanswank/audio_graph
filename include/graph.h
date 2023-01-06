#include <vector>
#include "xmodule.h"

struct audio_graph
{
    std::vector<int> visited;
    std::vector<int> process_order;
    std::vector<xmodule*> xmodules;
    
    void clear();
    
    // need to run clear() before search
    void DFS(int rootId);
};

template <typename T>
struct test
{
    std::vector<T*> things;
};
