#include "interpolation.h"

float lerp(float a, float b, float t) {
  return (1 - t) * a + t * b;
}
