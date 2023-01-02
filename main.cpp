#include <iostream>
#include <vector>

struct xmodule
{
    // Constructor
    // xmodule(int p_id, int p_num_inputs, int p_num_outputs) : id(p_id), num_inputs(p_num_inputs), num_ouputs(p_num_outputs) {}
    xmodule(int p_id) : id(p_id) {}

    // Add an input to the xmodule
    void add_input(int p_inputId)
    {
        input_ids.push_back(p_inputId);
    }

    // Add an output to the xmodule
    void add_output(int p_outputId)
    {
        output_ids.push_back(p_outputId);
    }

    // process the audio signal
    virtual void process() = 0;

    // ~xmodule() {};

    int id;

    // The number of inputs and outputs for the xmodule
    int num_inputs;
    int num_ouputs;

    // The input and output xmodule IDs
    std::vector<int> input_ids;
    std::vector<int> output_ids;
};

struct rt_midi_in : xmodule {
    rt_midi_in(int id) : xmodule(id) {}
    void process() override {std::cout << "id " << id << " midi in process" << std::endl;};
};

struct vst_plugin : xmodule {
    vst_plugin(int id) : xmodule(id) {}
    void process() override {std::cout <<  "id " << id << " vst plugin process" << std::endl;};
};

struct audio_output : xmodule {
    audio_output(int id) : xmodule(id) {}
    void process() override {std::cout <<  "id " << id << " final output" << std::endl;};
};

void DFS(int rootId, std::vector<xmodule*> &xmodules, std::vector<int> &visited)
{
    if (find(visited.begin(), visited.end(), rootId) != visited.end())
        return;                // Return if the node has already been visited
    visited.push_back(rootId); // Mark the node as visited
    // process the audio signal for the input xmodules of the current node
    for (int inputId : xmodules[rootId]->input_ids)
    {
        DFS(inputId, xmodules, visited);
    }
    xmodules[rootId]->process(); // process the audio signal for the current node
    // process the audio signal for the output xmodules of the current node
    for (int outputId : xmodules[rootId]->output_ids)
    {
        DFS(outputId, xmodules, visited); // Recursively process the audio signal for the output nodes
    }
}

int main()
{
    // Create a vector to store the xmodules
    std::vector<xmodule*> xmodules;

    xmodules.push_back(new rt_midi_in(0)); // rt midi in
    xmodules.push_back(new vst_plugin(1)); // vst plug
    xmodules.push_back(new vst_plugin(2)); // vst plug
    xmodules.push_back(new audio_output(3)); // output
    
    xmodules[0]->add_output(1);
    xmodules[0]->add_output(2);
    xmodules[1]->add_input(0);
    xmodules[1]->add_output(3);
    xmodules[2]->add_input(0);
    xmodules[2]->add_output(3);
    xmodules[3]->add_input(1);
    xmodules[3]->add_input(2);

    // Start the audio signal processing at the root node (in this case, the mixer xmodule)
    std::vector<int> visited;
    DFS(3, xmodules, visited);

    return 0;
}
