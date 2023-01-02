#include "vprint.h"
#include "portaudio.h"
#include <vector>
#include "xmodule.h"

class audio_interface
{
public:
    audio_interface();
    ~audio_interface();

    void init_devices(int sample_rate, int buffer_size, int input_device_id, int output_device_id);
    void scan_devices();
    void turn_on(PaStreamCallback* callback);
    void close_stream();
    
    std::vector<xmodule*> xmodules;
    std::vector<int> visited;
    void pass_userdata(std::vector<xmodule*>& p_xmodules, std::vector<int>& p_visited)
    {
        xmodules = p_xmodules;
        visited = p_visited;
    };

private:
    void set_param(bool is_capture, int dev_id);
    int sample_rate;
    int buffer_size;
    PaStreamParameters input_parameters;
    PaStreamParameters output_parameters;
    PaStream *stream;

};
