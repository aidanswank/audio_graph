// CjFilter.h
// created by Hagen Ueberfuhr
// www.klirrfactory.com
// This class is based on a video by Jacub Ciupinski
// https://www.youtube.com/watch?v=-WhbYLxEuMU
//
// License:
// This source code is provided as is, without warranty.
// You may copy and distribute verbatim copies of this document.
// You may modify and use this source code to create binary code for your own
// purposes, free or commercial.
//
// This class is made for VCV Rack.
// If you want to use it for VST or other music software that uses voltges between -1 and +1 V
// change:
// output = input *.2f;
// to
// output = input;
// and (two times)
// filterOut[i] = output * 5.f;
// to
// filterOut[i] = output;

#pragma once

class CjFilter {

  public:
    float output;
    int counter;
    float feedback[4][16];
    float filterOut[4];
    // This method returns a pointer to the filterOut[4] array
    float * doFilter(float input,float cut, float res);
    float damp;

    CjFilter(){
      output = 0.f;
      counter = 0;
      damp = 1.f;
      for (int i = 0; i<4; i++){
        filterOut[i] = 0.f;
        for (int j = 0; j<16; j++){
          feedback[i][j] = 0.f;
        }
      }
    };
  ~CjFilter(){
  };
};
