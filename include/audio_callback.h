#include "audio_interface.h"
#include "global_transport.h"

extern global_transport g_transport;

int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                          void *userData );
