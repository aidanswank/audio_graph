#include "allpass_filter.h"
#include <cmath>

AllpassFilter::AllpassFilter(double gain, int delayMillis, double sampleRate) {
    g = gain;
    delay = static_cast<int>(delayMillis * sampleRate / 1000.0);
    bufferSize = delay + 1;
    delayBuffer = new double[bufferSize];
    writeIndex = 0;
    readIndex = delay;

    // Initialize delay buffer with zeros
    for (int i = 0; i < bufferSize; i++) {
        delayBuffer[i] = 0.0;
    }
}

AllpassFilter::~AllpassFilter() {
    delete[] delayBuffer;
}

double AllpassFilter::process(double input) {
    double output = (-g * input) + delayBuffer[(readIndex - delay + bufferSize) % bufferSize] + (g * delayBuffer[(readIndex - delay + bufferSize) % bufferSize]);

    // Update delay buffer
    delayBuffer[writeIndex] = input + g * output;
    writeIndex = (writeIndex + 1) % bufferSize;
    readIndex = (readIndex + 1) % bufferSize;

    return output;
}
