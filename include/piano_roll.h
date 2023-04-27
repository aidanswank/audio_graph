#pragma once

#include "MidiFile.h"
#include "ImGui.h"
#include "vprint.h"
#include <cmath>
#include "global_transport.h"

int snap(float num, int by);

void piano_roll_window(bool *is_open, smf::MidiFile& midi_file);
void pattern_editor_window(bool *is_open);
