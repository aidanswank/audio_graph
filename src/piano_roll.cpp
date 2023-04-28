#include "piano_roll.h"

int piano_keys[12]={255,0,255,0,255,255,0,255,0,255,0,255};
std::string note_names[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

int snap(float num, int by)
{
    return round(num/by)*by;
}

extern global_transport g_transport;

#include <filesystem>
namespace fs = std::filesystem;
void PrintDirectory(const fs::path& directory)
{
    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (fs::is_directory(entry))
        {
            if (ImGui::TreeNode(entry.path().filename().string().c_str()))
            {
                PrintDirectory(entry);
                ImGui::TreePop();
            }
        }
        else
        {
            ImGui::Text("%s", entry.path().filename().string().c_str());
        }
    }
}

const int kPatternWidth = 8;
const int kPatternHeight = 8;
bool pattern[kPatternHeight][kPatternWidth] = {}; // Initialize all cells to false

void draw_pattern_editor()
{
    const float kButtonSize = 16.0f;
    const ImVec2 kButtonSpacing(2.0f, 2.0f);
    for (int y = 0; y < kPatternHeight; y++)
    {
        for (int x = 0; x < kPatternWidth; x++)
        {
            ImGui::PushID(y * kPatternWidth + x);
            ImGui::PushStyleColor(ImGuiCol_Button, pattern[y][x] ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, pattern[y][x] ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, pattern[y][x] ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
            if (ImGui::Button("##cell", ImVec2(kButtonSize, kButtonSize)))
            {
                pattern[y][x] = !pattern[y][x];
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine(0.0f, kButtonSpacing.x);
            ImGui::PopID();
        }
        ImGui::Dummy(ImVec2(0.0f, kButtonSpacing.y));
    }
}

#include "imgui_internal.h"

bool inc_dec_button(ImVec2 pos, ImVec2 button_size, ImVec2 small_button_size, int& pattern_num)
{
    bool is_clicked = false;
    
    // convert num to str
    char label[16];
    sprintf(label, "%d", pattern_num);
    
    ImGui::SetCursorPos(pos);
    if(ImGui::ButtonEx(label, button_size, ImGuiButtonFlags_AllowItemOverlap))
    {
        is_clicked = true;
    }

    ImGui::SetItemAllowOverlap();
    ImGui::SetCursorPos(ImVec2(pos.x + button_size.x - small_button_size.x, pos.y));
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    
    
    // inc and dec stuff
    if(ImGui::Button("+", small_button_size))
    {
        pattern_num++;
    }
    
    ImGui::SetCursorPos(ImVec2(pos.x+button_size.x-small_button_size.x, pos.y+button_size.y-small_button_size.y));

    if(ImGui::Button("-", small_button_size))
    {
        pattern_num--;
    }

    // dont go negative
    if(pattern_num<0)
    {
        pattern_num=0;
    }
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    
    return is_clicked;
}

//int pattern_nums[16] = {}; // init all to zero
std::vector<int> pattern_nums(16, 0);
//int is_patterned_disabled[16] = {}; // init all to zero

// imgui doesnt do this so i added it
ImVec2 operator+(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x + b.x, a.y + b.y);
}

void pattern_editor_window(bool *is_open)
{
    ImGui::Begin("pattern editor", is_open, ImGuiWindowFlags_HorizontalScrollbar);
    
    ImVec2 winpos = ImGui::GetCursorPos();
    
//    print("winpos",winpos.x,winpos.y);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
//    ImVec2 window_pos = ImGui::GetWindowPos();
    
    ImVec2 relative_win_pos = ImGui::GetWindowPos();
    relative_win_pos.x -= ImGui::GetScrollX();
    relative_win_pos.y -= ImGui::GetScrollY();
    
    ImVec2 rect_min = relative_win_pos+ImVec2(50, 50); // Adjust the values here to position the rectangle
    ImVec2 rect_max = relative_win_pos+ImVec2(100, 100); // Adjust the values here to position the rectangle

    ImVec4 color = ImVec4(1.0f, 0.5f, 1.0f, 0.25); //RGBA color values ranging from 0 to 1

    draw_list->AddRect(rect_min, rect_max, ImGui::GetColorU32(color));
    
    // Define the vertices of the polygon
    ImVec2 vertices[] = {
        relative_win_pos+ImVec2(100, 100),
        relative_win_pos+ImVec2(150, 50),
        relative_win_pos+ImVec2(200, 100),
        relative_win_pos+ImVec2(150, 150)
    };
    int num_vertices = 4;

    // Set the color of the polygon
//    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //RGBA color values ranging from 0 to 1

    // Draw the filled convex polygon
    draw_list->AddConvexPolyFilled(vertices, num_vertices, ImGui::GetColorU32(color));
    
    for(int i = 0; i < 16; i++)
    {
        ImGui::PushID(i);
        
        ImVec2 btn_main_sz = ImVec2(48,48);
        ImVec2 btn_notches_sz = ImVec2(12,16);
        int pad = 2;

        ImVec2 pos;
        pos.x = winpos.x + (i * (btn_main_sz.x + pad));
        pos.y = winpos.y;
        if(inc_dec_button(pos, btn_main_sz, btn_notches_sz, pattern_nums[i]))
        {
            print("tog",i);
        }
        
        ImGui::PopID();
    }
    
//    draw_pattern_editor();
    
    ImGui::End();
}

void piano_roll_window(bool *is_open, smf::MidiFile& midi_file)
{

    smf::MidiEventList& midiTrack = midi_file[0];

    static int sel = -1; // track note index??
    static int noteleft_dragIdx = -1;
    static int noteright_dragIdx = -1;
    static smf::MidiEvent mynote;
    static smf::MidiEvent lastNote;
    static smf::MidiEvent lastEventClicked;
    static float ticks_per_colum = 1;
    static float note_height = 8;

    float divsize = (96.f / 4);

    
    ImGui::Begin("toptoolbar", is_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);
    
    
//    draw_pattern_editor();
//    if (ImGui::TreeNode("Contents"))
//    {
//        PrintDirectory(currentDirectory);
//        ImGui::TreePop();
//    }
//
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
    
    ImGui::SliderFloat("width", &ticks_per_colum, 0.25f, 32);
//     ImGui::SameLine();
    ImGui::SliderFloat("height", &note_height, 1.f, 32);
    
    ImGui::End();

    // ImGui::Begin("sidetoolbar",isOpen,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_HorizontalScrollbar);
    // ImGui::VSliderFloat("height",ImVec2(16,200),&noteHeight,1.f,16);
    // ImGui::End();

    ImGui::Begin("piano roll", is_open, ImGuiWindowFlags_HorizontalScrollbar);

//    static ImGuiAxis toolbar1_axis = ImGuiAxis_X;
//    DockingToolbar("Toolbar1", &toolbar1_axis);
//    static ImGuiAxis toolbar2_axis = ImGuiAxis_Y;
//    DockingToolbar("Toolbar2", &toolbar2_axis);
    // const ImVec2 p = ImGui::GetCursorScreenPos();
    
//    print("is window focus %i", ImGui::IsWindowFocused());

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

//    const ImVec2 wp = ImGui::GetWindowPos();

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
    
    ImVec2 relative_win_pos = ImGui::GetWindowPos();
    relative_win_pos.x -= ImGui::GetScrollX();
    relative_win_pos.y -= ImGui::GetScrollY();

    ImVec2 mouse_pos = ImGui::GetMousePos();
    // print(mouse_pos.x,mouse_pos.y-relativeWindowPos.y);

    // float relativeMouse.y = mouse_pos.y-relativeWindowPos.y;
    ImVec2 relative_mouse_pos;
    relative_mouse_pos.x = mouse_pos.x-relative_win_pos.x;
    relative_mouse_pos.y = mouse_pos.y-relative_win_pos.y;

    // grid grids
    
//    print(ImGui::GetWindowContentRegionMax().x+ImGui::GetScrollX(),ImGui::GetContentRegionMax().y);
    float space_between_grids = (divsize / ticks_per_colum);
    int grid_loop_size = (content_pane_x);
//    grid_loop_size = grid_loop_size;
//    print("grid loop sz", grid_loop_size);
    
    for (int i = 0; i < 100; i++)
    {
        float x = space_between_grids * i;
        draw_list->AddLine(
            ImVec2(relative_win_pos.x + x + 32, relative_win_pos.y + 0),
            ImVec2(relative_win_pos.x + x + 32,
            relative_win_pos.y + ImGui::GetWindowHeight()+ImGui::GetScrollY()),
            ImColor(100, 100, 100, 50)
        );
    }
    
    // draw playhead
    {
        float x = g_transport.midi_tick_count * ticks_per_colum;
//        ImGui::SetScrollX(x);
        
        draw_list->AddLine(
                           ImVec2(relative_win_pos.x + x + 32, relative_win_pos.y + 0),
                           ImVec2(relative_win_pos.x + x + 32,
                                  relative_win_pos.y + ImGui::GetWindowHeight()+ImGui::GetScrollY()),
                           ImColor(200, 245, 66, 255)
                           );
    }
    
//    print("ticks",g_transport.midi_tick_count);
    
    // draw all notes
    for(int i = 0; i < midiTrack.size(); i++)
    {
        // print(midiTrack[i].tick);
        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(255,0,0,255));
        ImGui::PopStyleColor();

        smf::MidiEvent *event = &midiTrack[i];
        
        if(event->isNoteOn())
        {
//             print("pitch",event->pitch_bend);
            
            float duration = event->getTickDuration();
            float tick = event->tick;
            float key = event->getKeyNumber();
            // set up note rectangle dimensions
            float note_w = duration / ticks_per_colum;
            float note_x = (tick / ticks_per_colum) + 32;
            // int noteRange = track.maxNote - track.minNote;
            // // float note_y = ((noteRange) - (track.notes[i].key - track.minNote)) * noteHeight;
            float note_y = (127-key) * note_height;


            // smf::MidiEvent *endNote = track.notes[i].endNote;
            int startX = ((float)event->tick / ticks_per_colum) + 32;
            int endX = ((float)event->tick / ticks_per_colum) + 32 + note_w - 8;

            // print(dur);
            ImVec2 startbtnsize = ImVec2(8, note_height);
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

            ImVec2 size = ImVec2(note_w, note_height);
            ImGui::SetCursorPos(ImVec2(note_x,note_y));
            ImVec2 button_size(note_w, note_height);
            ImGui::PushID(i);
            // ImGui::PushStyleColor(ImGuiCol_Button,key_color);
            ImGui::Button(" ",button_size);
            // ImGui::PopStyleColor();
            ImGui::PopID();
            
//            ImVec2 rect_min = relative_win_pos+ImVec2(note_x, note_y); // Adjust the values here to position the rectangle
//            ImVec2 rect_max = relative_win_pos+ImVec2(note_x+note_w, note_y+noteHeight); // Adjust the values here to position the rectangle
//
//            ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //RGBA color values ranging from 0 to 1
//
//            draw_list->AddRect(rect_min, rect_max, ImGui::GetColorU32(color));
            
            // draw button
            ImVec2 rect_min = relative_win_pos + ImVec2(note_x, note_y); // Adjust the values here to position the rectangle
            ImVec2 rect_max = relative_win_pos + ImVec2(note_x + note_w, note_y + note_height); // Adjust the values here to position the rectangle
            ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 0.35f); //RGBA color values ranging from 0 to 1

            // Define the vertices of the rectangle
            ImVec2 vertices[4];
            vertices[0] = ImVec2(rect_min.x, rect_min.y-event->pitch_bend_a);
            vertices[1] = ImVec2(rect_max.x, rect_min.y-event->pitch_bend_b);
            vertices[2] = ImVec2(rect_max.x, rect_max.y-event->pitch_bend_b);
            vertices[3] = ImVec2(rect_min.x, rect_max.y-event->pitch_bend_a);

            // Draw the filled rectangle
//            draw_list->AddConvexPolyFilled(vertices, 4, ImGui::GetColorU32(color));
            
            draw_list->AddLine(
                                   relative_win_pos+ImVec2(note_x,note_y-event->pitch_bend_a+(note_height/2)),
                                   relative_win_pos+ImVec2(note_x+note_w,note_y-event->pitch_bend_b+(note_height/2)),
                                   ImGui::GetColorU32(ImVec4(1,0.1,1,1))
                               );

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
                int offset = (new_pos.x * ticks_per_colum);
                int dur = midiTrack[sel].getTickDuration();
                int s = snap(lastEventClicked.tick + offset,divsize);
                midiTrack[sel].tick = s;

                // move other note
                midiTrack[sel].getLinkedEvent()->tick = dur + s;
                // print(midiTrack[sel].tick,midiTrack[sel].getLinkedEvent()->tick);

                int new_key = lastEventClicked.getKeyNumber() + (round(new_pos.y / note_height) * -1);
                midiTrack[sel].setKeyNumber(new_key);
                midiTrack[sel].getLinkedEvent()->setKeyNumber(new_key);

            } else {
                
//                int offset = (new_pos.x * ticksPerColum);
//                int left_tick = midiTrack[noteleft_dragIdx].tick;
//                int right_tick = midiTrack[noteright_dragIdx].tick;

                // begin note button
                if(noteleft_dragIdx != -1)
                {
//                    print("new drag pos left",new_pos.x,new_pos.y);
                    int offset = (new_pos.x * ticks_per_colum);
                    int left_tick = snap(lastEventClicked.tick + offset,divsize);
//                    print(left_tick,right_tick);

                    midiTrack[noteleft_dragIdx].tick = left_tick;
                    midiTrack[noteleft_dragIdx].pitch_bend_a = -new_pos.y;
                }
                
                // end note button
                if(noteright_dragIdx != -1)
                {
//                    print("new drag pos right",new_pos.x,new_pos.y);
                    int offset = (new_pos.x * ticks_per_colum);
                    int s = snap(lastEventClicked.tick + offset,divsize);
                    // print(s);
                    midiTrack[noteright_dragIdx].getLinkedEvent()->tick = s;
                    midiTrack[noteright_dragIdx].pitch_bend_b = -new_pos.y;
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

        ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX(),note_height*i2));
        ImVec2 button_size(32, note_height);
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

    if(ImGui::IsWindowHovered())
    {
        // note debuggggg
        float note_snap_y = floor(relative_mouse_pos.y/note_height);
        int notenum = 127-int(note_snap_y);
        int octave = int(notenum / 12) - 1;
        int note = (notenum % 12);
        // print(note_names[note]);
        if(note>0) // so it doesnt draw past window area
        {
            ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX()+38,note_snap_y*note_height));
        }
        ImGui::Text("%s %i", note_names[note].c_str(), notenum);
    }

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
                
                int key = (floor(relative_mouse_pos.y/note_height)*-1) - 1; // why is this negative?
                print(key);
                
                int tick = (mouse_pos.x-32+ImGui::GetScrollX()-ImGui::GetWindowPos().x) * ticks_per_colum;
                
                //             tick = snap(tick,divsize);
                tick = floor(tick/divsize)*divsize;
                
                smf::MidiEvent eventOn;
                eventOn.tick = tick;
                eventOn.track = 0;
                eventOn.makeNoteOn(0, key, 60);
                midi_file.addEvent(eventOn);
                
                smf::MidiEvent eventOff;
                eventOff.tick = tick+(divsize*1);
                eventOff.track = 0;
                eventOff.makeNoteOff(0,key,0);
                midi_file.addEvent(eventOff);
                
                midi_file.linkNotePairs();
                
                // event.makeNoteOff();
                // midiFile.addEvent(event);
                print("insert note",mouse_pos.x,mouse_pos.y,tick);
            }
        }
    }

//    draw_list->AddRect( vMin, vMax, IM_COL32( 255, 0, 0, 255 ) );
    
    ImGui::End();
}
