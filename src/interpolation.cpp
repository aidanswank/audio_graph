#include "interpolation.h"
#include <math.h>

float lerp(float a, float b, float t) {
  return (1 - t) * a + t * b;
}

// http://paulbourke.net/miscellaneous/interpolation/
double CosineInterpolate(
   double y1,double y2,
   double mu)
{
   double mu2;

   mu2 = (1-cos(mu*M_PI))/2;
   return(y1*(1-mu2)+y2*mu2);
}
