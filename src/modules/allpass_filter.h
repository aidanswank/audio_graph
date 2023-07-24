class AllpassFilter {
private:
    double g;       // Gain
    int delay;      // Delay in milliseconds
    double* delayBuffer;  // Circular delay buffer
    int bufferSize; // Size of the delay buffer
    int writeIndex; // Current write index in the buffer
    int readIndex;  // Current read index in the buffer

public:
    AllpassFilter(double gain, int delayMillis, double sampleRate);
    ~AllpassFilter();
    double process(double input);
};
