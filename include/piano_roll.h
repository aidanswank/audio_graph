#pragma once

#include "MidiFile.h"
#include "ImGui.h"
#include "vprint.h"
#include <cmath>
#include "global_transport.h"

int snap(float num, int by);

void piano_roll_window(bool *isOpen, smf::MidiFile& midiFile);
