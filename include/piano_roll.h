#pragma once

#include "MidiFile.h"
#include "ImGui.h"
#include "vprint.h"
#include <cmath>
#include "global_transport.h"

int snap(float num, int by);

void sequencer_controls_window();
void piano_roll_window(bool *is_open, std::map<int, smf::MidiFile> &midi_map, int node_id);
void piano_roll_window2(bool *is_open, std::vector<midi_note_message> &midi_track);
void pattern_editor_window(bool *is_open);
void clip_timeline_window(bool *is_open);

