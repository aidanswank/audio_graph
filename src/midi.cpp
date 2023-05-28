#include "midi.h"

double midi2freq(int n)
{
    double f = 440.0 * pow(2, ((double)n - 69) / 12.0);

    return f;
};

double midi2speed(int n) // if sample tuned to A, returns speed from 0.0 to 1.0
{
    double f = 1.0 * pow(2, ((double)n - 69) / 12.0);

    return f;
};
