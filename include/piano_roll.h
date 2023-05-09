#pragma once

#include "MidiFile.h"
#include "ImGui.h"
#include "vprint.h"
#include <cmath>
#include "global_transport.h"
#include "xmodule.h"
#include "midi_sequencer_module.h"

int snap(float num, int by);

void sequencer_controls_window();
void piano_roll_window(bool *is_open, smf::MidiFile& midi_file);
void piano_roll_window2(bool *is_open, std::vector<midi_note_message> &midi_track);
void pattern_editor_window(bool *is_open,audio_graph<xmodule*>* graph);
void clip_timeline_window(bool *is_open);

