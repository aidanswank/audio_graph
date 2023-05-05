#include "midi.h"

double midi2freq(int n)
{
    double f = 440.0 * pow(2, ((double)n - 69) / 12.0);

    return f;
};
