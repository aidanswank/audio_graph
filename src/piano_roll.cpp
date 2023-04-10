#include "piano_roll.h"

int piano_keys[12]={255,0,255,0,255,255,0,255,0,255,0,255};
std::string note_names[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

int snap(float num, int by)
{
    return round(num/by)*by;
}

extern global_transport g_transport;

void piano_roll_window(bool *isOpen, smf::MidiFile& midiFile)
{

    smf::MidiEventList& midiTrack = midiFile[0];

    static int sel = -1; // track note index??
    static int noteleft_dragIdx = -1;
    static int noteright_dragIdx = -1;
    static smf::MidiEvent mynote;
    static smf::MidiEvent lastNote;
    static smf::MidiEvent lastEventClicked;
    static float ticksPerColum = 1;
    static float noteHeight = 8;

    float divsize = (96.f / 4);

    
    ImGui::Begin("toptoolbar", isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);
    
//    static char buf[16];
//    snprintf(buf, 16, "%f", g_transport.tempo);
    ImGui::PushItemWidth(35.0f);
    if(ImGui::Button("-"))
    {
        g_transport.tempo-=5;
    }
    ImGui::SameLine();
    ImGui::InputFloat("##tempo", &g_transport.tempo);
    ImGui::SameLine();
    if(ImGui::Button("+"))
    {
        g_transport.tempo+=5;
    }
    ImGui::SameLine();
    if(ImGui::Button("PLAY"))
    {
        g_transport.is_playing=true;
    }
    ImGui::SameLine();
    if(ImGui::Button("PAUSE"))
    {
        g_transport.is_playing=false;
    }
    ImGui::SameLine();
    if(ImGui::Button("RESET"))
    {
        g_transport.sample_count=0;
        g_transport.midi_tick_count=0;
        g_transport.current_seconds=0;
    }
//    ImGui::SameLine();
    ImGui::Text("sample %i midi tick %f seconds%f",g_transport.sample_count,g_transport.midi_tick_count,g_transport.current_seconds);
    ImGui::PopItemWidth();
    
    ImGui::SliderFloat("width", &ticksPerColum, 0.25f, 32);
//     ImGui::SameLine();
    ImGui::SliderFloat("height", &noteHeight, 1.f, 32);
    
    ImGui::End();

    // ImGui::Begin("sidetoolbar",isOpen,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_HorizontalScrollbar);
    // ImGui::VSliderFloat("height",ImVec2(16,200),&noteHeight,1.f,16);
    // ImGui::End();

    ImGui::Begin("piano roll", isOpen, ImGuiWindowFlags_HorizontalScrollbar);

//    static ImGuiAxis toolbar1_axis = ImGuiAxis_X;
//    DockingToolbar("Toolbar1", &toolbar1_axis);
//    static ImGuiAxis toolbar2_axis = ImGuiAxis_Y;
//    DockingToolbar("Toolbar2", &toolbar2_axis);
    // const ImVec2 p = ImGui::GetCursorScreenPos();
    
//    print("is window focus %i", ImGui::IsWindowFocused());

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    const ImVec2 wp = ImGui::GetWindowPos();

    // print(p.x,p.y);
    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();

    
    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    
    vMin.x += ImGui::GetWindowPos().x + ImGui::GetScrollX();
    vMin.y += ImGui::GetWindowPos().y + ImGui::GetScrollY();
    vMax.x += ImGui::GetWindowPos().x + ImGui::GetScrollX();
    vMax.y += ImGui::GetWindowPos().y + ImGui::GetScrollY();
    
//    print("vMin",vMin.x,vMin.y,"vMax",vMax.x,vMax.y);
//    print("vMaxX-vMinX",vMax.x-vMin.x);
    float content_pane_x = vMax.x-vMin.x;
    
    ImVec2 relativeWindowPos = ImGui::GetWindowPos();
    relativeWindowPos.x -= ImGui::GetScrollX();
    relativeWindowPos.y -= ImGui::GetScrollY();

    ImVec2 mouse_pos = ImGui::GetMousePos();
    // print(mouse_pos.x,mouse_pos.y-relativeWindowPos.y);

    // float relativeMouse.y = mouse_pos.y-relativeWindowPos.y;
    ImVec2 relativeMouse;
    relativeMouse.x = mouse_pos.x-relativeWindowPos.x;
    relativeMouse.y = mouse_pos.y-relativeWindowPos.y;

    // grid grids
    
//    print(ImGui::GetWindowContentRegionMax().x+ImGui::GetScrollX(),ImGui::GetContentRegionMax().y);
    float space_between_grids = (divsize / ticksPerColum);
    int grid_loop_size = (content_pane_x);
//    grid_loop_size = grid_loop_size;
//    print("grid loop sz", grid_loop_size);
    
    for (int i = 0; i < 100; i++)
    {
        float x = space_between_grids * i;
        draw_list->AddLine(
            ImVec2(relativeWindowPos.x + x + 32, relativeWindowPos.y + 0),
            ImVec2(relativeWindowPos.x + x + 32,
            relativeWindowPos.y + ImGui::GetWindowHeight()+ImGui::GetScrollY()),
            ImColor(100, 100, 100, 50)
        );
    }
    
    // draw playhead
    {
        float x = g_transport.midi_tick_count * ticksPerColum;
//        ImGui::SetScrollX(x);
        
        draw_list->AddLine(
                           ImVec2(relativeWindowPos.x + x + 32, relativeWindowPos.y + 0),
                           ImVec2(relativeWindowPos.x + x + 32,
                                  relativeWindowPos.y + ImGui::GetWindowHeight()+ImGui::GetScrollY()),
                           ImColor(200, 245, 66, 255)
                           );
    }
    
//    print("ticks",g_transport.midi_tick_count);

    for(int i = 0; i < midiTrack.size(); i++)
    {
        // print(midiTrack[i].tick);
        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(255,0,0,255));
        ImGui::PopStyleColor();

        smf::MidiEvent *event = &midiTrack[i];

        if(event->isNoteOn())
        {
            // print("testttt");
            float duration = event->getTickDuration();
            float tick = event->tick;
            float key = event->getKeyNumber();
            // set up note rectangle dimensions
            float note_w = duration / ticksPerColum;
            float note_x = (tick / ticksPerColum) + 32;
            // int noteRange = track.maxNote - track.minNote;
            // // float note_y = ((noteRange) - (track.notes[i].key - track.minNote)) * noteHeight;
            float note_y = (127-key) * noteHeight;


            // smf::MidiEvent *endNote = track.notes[i].endNote;
            int startX = ((float)event->tick / ticksPerColum) + 32;
            int endX = ((float)event->tick / ticksPerColum) + 32 + note_w - 8;

            // print(dur);
            ImVec2 startbtnsize = ImVec2(8, noteHeight);
            ImVec2 startbtnpos = ImVec2(startX, note_y);
            ImVec2 endbtnpos = ImVec2(endX, note_y);

            ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(255,0,0,255));

            ImGui::SetCursorPos(startbtnpos);
            ImGui::Button(std::to_string(i).c_str(),startbtnsize);
            if (ImGui::IsItemClicked())
            {
                 print("start note",i);
                noteleft_dragIdx = i;
                lastEventClicked = midiTrack[noteleft_dragIdx];
            }
            ImGui::SetCursorPos(endbtnpos);
            ImGui::Button(std::to_string(i).c_str(),startbtnsize);
            // selects note
            if (ImGui::IsItemClicked())
            {
                // print("start??",i);
                noteright_dragIdx = i;
                lastEventClicked = *midiTrack[noteright_dragIdx].getLinkedEvent();
            }
            ImGui::PopStyleColor();

            ImVec2 size = ImVec2(note_w, noteHeight);
            ImGui::SetCursorPos(ImVec2(note_x,note_y));
            ImVec2 button_size(note_w, noteHeight);
            ImGui::PushID(i);
            // ImGui::PushStyleColor(ImGuiCol_Button,key_color);
            ImGui::Button(" ",button_size);
            // ImGui::PopStyleColor();
            ImGui::PopID();

            // selects note
            if (ImGui::IsItemClicked())
            {
                sel = i;
                // mynote = track.notes[sel];
                lastEventClicked = midiTrack[sel];
//                print("sel",sel);
            }
            //double click remove note;
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && sel!=-1)
            {
                midiTrack[sel].clear();
//                midiTrack.removeEmpties();
            }
        };

        // handles moving note
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            // print("sel", sel, " ", ImGui::GetMouseDragDelta().x, " ", ImGui::GetMouseDragDelta().y);
            ImVec2 new_pos = ImGui::GetMouseDragDelta();
            // points[i].x = pos.x;
            
            // main selection
            if (sel != -1)
            {
                // ((float)(track.tpq / ticksPerColum)/4)
                int offset = (new_pos.x * ticksPerColum);
                int dur = midiTrack[sel].getTickDuration();
                int s = snap(lastEventClicked.tick + offset,divsize);
                midiTrack[sel].tick = s;

                // move other note
                midiTrack[sel].getLinkedEvent()->tick = dur + s;
                // print(midiTrack[sel].tick,midiTrack[sel].getLinkedEvent()->tick);

                int new_key = lastEventClicked.getKeyNumber() + (round(new_pos.y / noteHeight) * -1);
                midiTrack[sel].setKeyNumber(new_key);
                midiTrack[sel].getLinkedEvent()->setKeyNumber(new_key);

            } else {
                
//                int offset = (new_pos.x * ticksPerColum);
//                int left_tick = midiTrack[noteleft_dragIdx].tick;
//                int right_tick = midiTrack[noteright_dragIdx].tick;

                // begin note button
                if(noteleft_dragIdx != -1)
                {
                    print(new_pos.x,new_pos.y);
                    int offset = (new_pos.x * ticksPerColum);
                    int left_tick = snap(lastEventClicked.tick + offset,divsize);
//                    print(left_tick,right_tick);

                    midiTrack[noteleft_dragIdx].tick = left_tick;
                }
                
                // end note button
                if(noteright_dragIdx != -1)
                {
                    print(new_pos.x,new_pos.y);
                    int offset = (new_pos.x * ticksPerColum);
                    int s = snap(lastEventClicked.tick + offset,divsize);
                    // print(s);
                    midiTrack[noteright_dragIdx].getLinkedEvent()->tick = s;
                }
            }
            
        }
    }
    
    // float hey = (96.f / 4);

    // draw piano buttons
    for(int i = 0; i < 128; i++)
    {
        int i2 = 127-i;
        int bw = piano_keys[i%12];
        uint32_t key_color = ImColor(bw,bw,bw);

        ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX(),noteHeight*i2));
        ImVec2 button_size(32, noteHeight);
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_Button,key_color);
        ImGui::Button(" ",button_size);
        // if(ImGui::IsItemHovered())
        // {
        //     // ImGui::PushStyleColor(ImGuiCol_Button,ImColor(255,0,255));
        //     print("hovering...");
        //     ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX()+32,noteHeight*i2));
        //     ImGui::Text(std::to_string(i).c_str());
        // }
        ImGui::PopStyleColor();
        ImGui::PopID();

//         draw_list->AddRectFilled(
//                 ImVec2(ImGui::GetWindowPos().x,
//             relativeWindowPos.y + (i*noteHeight)),
//                 ImVec2(ImGui::GetWindowPos().x + 32,
//             relativeWindowPos.y + noteHeight + (i*noteHeight)),
//             key_color, 1.0f,
//             ImDrawCornerFlags_All
//         );

    }



    // note debuggggg
    float note_snap_y = floor(relativeMouse.y/noteHeight);
    int notenum = 127-int(note_snap_y);
    int octave = int(notenum / 12) - 1;
    int note = (notenum % 12);
    // print(note_names[note]);
    if(note>0) // so it doesnt draw past window area
    {
        ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX()+38,note_snap_y*noteHeight));
    }
    ImGui::Text("%s %i", note_names[note].c_str(), notenum);

    // resets note selection
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
//        print("ImGuiMouseButton_Left released");
        sel = -1;
        noteleft_dragIdx = -1;
        noteright_dragIdx = -1;
    }
    
    if(ImGui::IsWindowFocused() && mouse_pos.y<vMax.y && mouse_pos.x<vMax.x && mouse_pos.y>vMin.y && mouse_pos.x>vMin.x) // need focus to insert note and not clicking outside scrollbar bounds
    {
        // insert note if nothing is selected and clicked
        if(sel==-1&&noteleft_dragIdx==-1&&noteright_dragIdx==-1)
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                
                int key = (floor(relativeMouse.y/noteHeight)*-1) - 1; // why is this negative?
                print(key);
                
                int tick = (mouse_pos.x-32+ImGui::GetScrollX()-ImGui::GetWindowPos().x) * ticksPerColum;
                
                //             tick = snap(tick,divsize);
                tick = floor(tick/divsize)*divsize;
                
                smf::MidiEvent eventOn;
                eventOn.tick = tick;
                eventOn.track = 0;
                eventOn.makeNoteOn(0, key, 60);
                midiFile.addEvent(eventOn);
                
                smf::MidiEvent eventOff;
                eventOff.tick = tick+(divsize*1);
                eventOff.track = 0;
                eventOff.makeNoteOff(0,key,0);
                midiFile.addEvent(eventOff);
                
                midiFile.linkNotePairs();
                
                // event.makeNoteOff();
                // midiFile.addEvent(event);
                print("insert note",mouse_pos.x,mouse_pos.y,tick);
            }
        }
    }

//    draw_list->AddRect( vMin, vMax, IM_COL32( 255, 0, 0, 255 ) );
    
    ImGui::End();
}
