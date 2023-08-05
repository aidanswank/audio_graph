#include "audio_interface.h"

audio_interface::audio_interface()
{
    PaError err;
    err = Pa_Initialize();
    if( err != paNoError ) {
        print("init err");
    }
}

audio_interface::~audio_interface()
{
//	printf("audio interface shutting down\n");
    PaError err;
    err = Pa_Terminate();
    if( err != paNoError ) { print("error closing"); }

}

void audio_interface::set_param(bool is_capture, int dev_id)
{
    const PaDeviceInfo* device_info = Pa_GetDeviceInfo( dev_id );
    PaStreamParameters &param = is_capture ? input_parameters : output_parameters;

    bzero( &param, sizeof( param ) ); //not necessary if you are filling in all the fields
    param.channelCount              = is_capture ? device_info->maxInputChannels : device_info->maxOutputChannels;
    param.device                    = dev_id;
    param.hostApiSpecificStreamInfo = NULL;
    param.sampleFormat              = paFloat32; // just cuz easy to work with
    param.suggestedLatency          = is_capture ? device_info->defaultLowInputLatency : device_info->defaultLowOutputLatency; /** Default latency values for interactive performance. */
    param.hostApiSpecificStreamInfo = NULL; //See you specific host's API docs for info on using this field 
}

void audio_interface::try_params()
{
    PaError err;
    // check if format is supported
    // remember laptop mic is MONO! if wrong channel count will fail
    err = Pa_IsFormatSupported( &input_parameters, &output_parameters, sample_rate );
    if( err == paFormatIsSupported )
    {
        printf( "Hooray! audio format is supported\n");
    }
    else
    {
        printf("Too Bad. audio format is not supported\n");
    }
}

void audio_interface::init_devices(int p_sample_rate, int p_buffer_size)
{
    sample_rate = p_sample_rate;
    buffer_size = p_buffer_size;
    PaError err;

//    set_param(true,input_device_id);
//    set_param(false,output_device_id);

//    try_params();
}

void audio_interface::scan_devices()
{
    int numDevices;
    numDevices = Pa_GetDeviceCount();
    if( numDevices < 0 )
    {
        printf( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
        // err = numDevices;
    }

    const PaDeviceInfo *deviceInfo;
    for( int i=0; i<numDevices; i++ )
    {
        deviceInfo = Pa_GetDeviceInfo( i );
        device_infos.push_back(*deviceInfo);
        print(i,"~",deviceInfo->name,"\ndefault sample rate:", deviceInfo->defaultSampleRate,"max inputs:",deviceInfo->maxInputChannels,"max outputs:",deviceInfo->maxOutputChannels,"\n");
    }
}

void audio_interface::turn_on(PaStreamCallback* callback)
{
    // PaStreamCallback
    PaError err;
    err = Pa_OpenStream(
                &stream,
                &input_parameters,
                &output_parameters,
                sample_rate,
                buffer_size,
                paNoFlag, //flags that can be used to define dither, clip settings and more
                callback, //your callback function
                (void*)this ); //data to be passed to callback. In C++, it is frequently (void *)this
//don't forget to check errors!

    err = Pa_StartStream( stream );

    if( err != paNoError ) {
        print("failed to open portaudio stream!", err);
    } else {
        print("portaudio opened stream");
    };
}

void audio_interface::close_stream()
{
    PaError err;
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to close PortAudio stream" << std::endl;
            // return 1;
    }
}
