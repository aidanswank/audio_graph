#include "piano_roll.h"

int piano_keys[12]={255,0,255,0,255,255,0,255,0,255,0,255};
std::string note_names[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

int snap(float num, int by)
{
    return round(num/by)*by;
}

extern global_transport g_transport;

//#include <filesystem>
//namespace fs = std::filesystem;
//void PrintDirectory(const fs::path& directory)
//{
//    for (const auto& entry : fs::directory_iterator(directory))
//    {
//        if (fs::is_directory(entry))
//        {
//            if (ImGui::TreeNode(entry.path().filename().string().c_str()))
//            {
//                PrintDirectory(entry);
//                ImGui::TreePop();
//            }
//        }
//        else
//        {
//            ImGui::Text("%s", entry.path().filename().string().c_str());
//        }
//    }
//}

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
std::vector<int> pattern_nums2(16, 0);
//int is_patterned_disabled[16] = {}; // init all to zero

#include "imgui_utils.h"

void notch_button(const char* label, ImVec2 btn_main_sz, ImVec2 btn_notches_sz)
{
    ImVec2 pos = ImGui::GetCursorPos();
    if(ImGui::ButtonEx(label, btn_main_sz, ImGuiButtonFlags_AllowItemOverlap))
    {
        print("big");
    }
//    pos3 = ImGui::GetCursorPos();

//    ImGui::SameLine();

    ImVec2 pos2 = ImGui::GetCursorPos();

    ImGui::SetCursorPos(pos);
    ImGui::SetItemAllowOverlap();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    if(ImGui::Button("+", btn_notches_sz))
    {
//            pattern_num++;
        print("plus");
    }
    ImGui::SetCursorPos(pos+ImVec2(0,btn_main_sz.y-btn_notches_sz.y));
    if(ImGui::Button("-", btn_notches_sz))
    {
//            pattern_num++;
        print("minus");
    }
    ImGui::PopStyleVar();
//    ImGui::SetCursorPos(pos-ImVec2(0,btn_main_sz.y));

    ImGui::SetCursorPos(pos);
    
}

void pattern_editor_window(bool *is_open, audio_graph<xmodule*>* graph)
{
    ImGui::Begin("pattern editor", is_open, ImGuiWindowFlags_HorizontalScrollbar);
    
    ImVec2 winpos = ImGui::GetCursorPos();
//
////    print("winpos",winpos.x,winpos.y);
//
//    ImDrawList* draw_list = ImGui::GetWindowDrawList();
////    ImVec2 window_pos = ImGui::GetWindowPos();
//
//    ImVec2 relative_win_pos = ImGui::GetWindowPos();
//    relative_win_pos.x -= ImGui::GetScrollX();
//    relative_win_pos.y -= ImGui::GetScrollY();
//
//    ImVec2 rect_min = relative_win_pos+ImVec2(50, 50); // Adjust the values here to position the rectangle
//    ImVec2 rect_max = relative_win_pos+ImVec2(100, 100); // Adjust the values here to position the rectangle
//
//    ImVec4 color = ImVec4(1.0f, 0.5f, 1.0f, 0.25); //RGBA color values ranging from 0 to 1
//
//    draw_list->AddRect(rect_min, rect_max, ImGui::GetColorU32(color));
//
//    // Define the vertices of the polygon
//    ImVec2 vertices[] = {
//        relative_win_pos+ImVec2(100, 100),
//        relative_win_pos+ImVec2(150, 50),
//        relative_win_pos+ImVec2(200, 100),
//        relative_win_pos+ImVec2(150, 150)
//    };
//    int num_vertices = 4;
//
//    // Set the color of the polygon
////    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //RGBA color values ranging from 0 to 1
//
//    // Draw the filled convex polygon
//    draw_list->AddConvexPolyFilled(vertices, num_vertices, ImGui::GetColorU32(color));
//
    ImVec2 btn_main_sz = ImVec2(48,48);
    ImVec2 btn_notches_sz = ImVec2(12,16);
    
//    for(int i = 0; i < 4; i++)
//    {
//        ImGui::PushID(i);
//
//        int pad = 2;
//        ImVec2 pos;
//        pos.x = winpos.x + (i * (btn_main_sz.x + pad));
//        pos.y = winpos.y;
//        if(inc_dec_button(pos, btn_main_sz, btn_notches_sz, pattern_nums[i]))
//        {
//            print("tog",i);
//        }
//
//        ImGui::PopID();
//    }
//
//    for(int i = 0; i < 4; i++)
//    {
//        ImGui::PushID(i+pattern_nums.size());
//
//        int pad = 2;
//        ImVec2 pos;
//        pos.x = winpos.x + (i * (btn_main_sz.x + pad));
//        pos.y = winpos.y + btn_main_sz.y + pad;
//        if(inc_dec_button(pos, btn_main_sz, btn_notches_sz, pattern_nums2[i]))
//        {
//            print("tog",i);
//        }
//
//        ImGui::PopID();
//    }
    
    int pattern_rows = 0;
    for ( auto it = g_transport.pattern_map.begin(); it != g_transport.pattern_map.end(); ++it  )
    {
//       std::cout << it->first << '\t' << it->second.size() << std::endl;
        
        ImVec2 pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(pos+ImVec2(0,16));
        ImGui::Text("%i",it->first);
        
        static std::vector<int> column_pattern_list = {0,0,0,0};

        for(int i = 0; i < it->second.size(); i++)
        {
            char pat_button_string[64];
            snprintf(pat_button_string, sizeof(pat_button_string), "pat_btn_%d,%d", i,pattern_rows);
            ImGui::PushID(pat_button_string);
            
            int pad = 2;
            ImVec2 pos;
            pos.x = 20+pad+winpos.x + (i * (btn_main_sz.x + pad));
            pos.y = winpos.y + (pattern_rows*(btn_main_sz.y + pad));
            bool flag=false;
            if(g_transport.current_pattern==i)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
//                std::cout << it->second[i] << std::endl;
                column_pattern_list[i] = it->second[i];
                flag=true;
            }
//            ImGui::Text("%i %i",it->first,it->second[i]);
//            print(it->second[i]);
            if(inc_dec_button(pos, btn_main_sz, btn_notches_sz, it->second[i] ))
            {
                print("open pattern!! ",it->second[i]);
                
//                midi_sequencer* seq_mod = (midi_sequencer*)graph->xmodules[ it->first ];
//                seq_mod->is_piano_roll_open=true;
//                seq_mod->current_pattern_open=it->second[i];
                g_transport.current_pattern_open = it->second[i];
                g_transport.current_column_open = i;
//                print("column",i);
            }
            if(flag)
            {
                ImGui::PopStyleColor(3);
            }
            
            ImGui::PopID();
        }
        
//        std::cout << std::endl;
//        for(int i = 0; i < 4; i++)
//        {
//            print(column_pattern_list[i]);
//        }

        pattern_rows++;
    }
////    ImVec2 pos;
//    for(int i = 0; i < 4; i++)
//    {
////        ImVec2 pos = ImGui::GetCursorPos();
////        if(ImGui::ButtonEx("555", btn_main_sz, ImGuiButtonFlags_AllowItemOverlap))
////        {
////            print("big");
////        }
////        pos3 = ImGui::GetCursorPos();
////
////        ImGui::SameLine();
////
////        ImVec2 pos2 = ImGui::GetCursorPos();
////
////        ImGui::SetCursorPos(pos);
////        ImGui::SetItemAllowOverlap();
////        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
////        if(ImGui::Button("+", btn_notches_sz))
////        {
//////            pattern_num++;
////            print("plus");
////        }
////        ImGui::SetCursorPos(pos+ImVec2(0,btn_main_sz.y-btn_notches_sz.y));
////        if(ImGui::Button("-", btn_notches_sz))
////        {
//////            pattern_num++;
////            print("minus");
////        }
////        ImGui::PopStyleVar();
////        ImGui::SetCursorPos(pos2);
//        notch_button("label", btn_main_sz, btn_notches_sz);
////        ImGui::Button("sssss");
//        ImGui::SameLine();
//
//    }
////    ImGui::SetCursorPos(pos);
//
//
//    ImGui::Button("testtinggg");
//
////    ImGui::NewLine();
//
//    static bool disabled = false;
//    if(ImGui::IsMouseDoubleClicked(0))
//    {
//        disabled = !disabled;
//        print("true??");
//    }
//    if(disabled)
//    {
//        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
//        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
//        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
//    }
//
//    for(int i = 0; i < 8; i++)
//    {
//
//        ImVec2 pos = ImGui::GetCursorPos();
//        if(ImGui::ButtonEx("okok", btn_main_sz, ImGuiButtonFlags_AllowItemOverlap))
//        {
//            print("big");
//        }
//        ImGui::SameLine();
//        ImVec2 pos2 = ImGui::GetCursorPos();
//
//
//        ImGui::SetCursorPos(pos);
//
//        ImGui::SetItemAllowOverlap();
//
//        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
//
//        if(ImGui::Button("+", btn_notches_sz))
//        {
////            pattern_num++;
//            print("plus");
//        }
//        ImGui::SetCursorPos(pos+ImVec2(0,btn_main_sz.y-btn_notches_sz.y));
//        if(ImGui::Button("-", btn_notches_sz))
//        {
////            pattern_num++;
//            print("minus");
//        }
//
//        ImGui::PopStyleVar();
//
////        ImGui::SameLine();
//        ImGui::SetCursorPos(pos2);
//
//
////        ImVec2 pos = ImGui::GetCursorPos();
////        ImGui::SetCursorPos(pos+ImVec2(0,btn_main_sz.x));
//
//    }
//    if(disabled)
//    {
//        ImGui::PopStyleVar(2);
//        ImGui::PopStyleColor(3);
//    }
//
////    if(ImGui::ButtonEx("okok2", btn_main_sz, ImGuiButtonFlags_AllowItemOverlap))
////    {
////
////    }
////    ImGui::SameLine();
//
////    draw_pattern_editor();
    
    ImGui::End();
}

//void clip_timeline_window(bool *is_open)
//{
//    ImGui::Begin("clip timeline", is_open, ImGuiWindowFlags_HorizontalScrollbar);
//    
//    ImVec2 cursor_pos = ImGui::GetCursorPos();
//    
//    ImDrawList* draw_list = ImGui::GetWindowDrawList();
//    
//    ImVec2 relative_win_pos = ImGui::GetWindowPos();
//    relative_win_pos.x -= ImGui::GetScrollX();
//    relative_win_pos.y -= ImGui::GetScrollY();
//    
//    ImVec2 rect_min = relative_win_pos+cursor_pos; // Adjust the values here to position the rectangle
//    ImVec2 rect_max = relative_win_pos+cursor_pos+ImVec2(50, 50); // Adjust the values here to position the rectangle
//
//    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0); //RGBA color values ranging from 0 to 1
//
//    draw_list->AddRect(rect_min, rect_max, ImGui::GetColorU32(color));
//    
//    if(ImGui::Button("##test",ImVec2(50,50)))
//    {
//        print("yo!");
//    };
//    
//    ImGui::End();
//}

//void piano_roll_window2(bool *is_open, std::vector<midi_note_message> &midi_track)
//{
////    smf::MidiEventList& midi_track = midi_file[0];
//
//    static int note_idx = -1; // track note index??
//    static int note_left_drag_idx = -1;
//    static int note_right_drag_idx = -1;
//    static midi_note_message last_event_clicked;
//    static float ticks_per_colum = 1;
//    static float note_height = 8;
//    static float current_hovered_note_num = -1;
//
//    static int grid_div = 4;
//    static float grid_sz = (96.f / grid_div);
//
//
//    ImGui::Begin("toptoolbar", is_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);
//
//
////    draw_pattern_editor();
////    if (ImGui::TreeNode("Contents"))
////    {
////        PrintDirectory(currentDirectory);
////        ImGui::TreePop();
////    }
////
////    static char buf[16];
////    snprintf(buf, 16, "%f", g_transport.tempo);
//    ImGui::PushItemWidth(35.0f);
//    if(ImGui::Button("-"))
//    {
//        g_transport.tempo-=5;
//    }
//    ImGui::SameLine();
//    ImGui::InputFloat("##tempo", &g_transport.tempo);
//    ImGui::SameLine();
//    if(ImGui::Button("+"))
//    {
//        g_transport.tempo+=5;
//    }
//    ImGui::SameLine();
//    if(ImGui::Button("PLAY"))
//    {
//        g_transport.is_playing=true;
//    }
//    ImGui::SameLine();
//    if(ImGui::Button("PAUSE"))
//    {
//        g_transport.is_playing=false;
//    }
//    ImGui::SameLine();
//    if(ImGui::Button("RESET"))
//    {
//        g_transport.sample_count=0;
//        g_transport.midi_tick_count=0;
//        g_transport.current_seconds=0;
//    }
//
//    ImGui::PopItemWidth();
//
//    ImGui::SameLine();
//
//
//    static int selected_item = 0;
//    const char* items[] = { "1", "2", "3" };
//    const int num_items = sizeof(items) / sizeof(items[0]);
//
//    ImGui::PushItemWidth(40.0f);
//    if (ImGui::BeginCombo("##div", items[selected_item])) {
//        for (int i = 0; i < num_items; ++i) {
//            const bool is_selected = (selected_item == i);
//            if (ImGui::Selectable(items[i], is_selected)) {
//                selected_item = i;
////                print(i);
//                if(i==0)
//                {
//                    grid_div = 4;
//                    grid_sz = (96.f / grid_div);
//                }
//                if(i==1)
//                {
//                    grid_div = 8;
//                    grid_sz = (96.f / grid_div);
//                }
//
//            }
//            if (is_selected) {
//                ImGui::SetItemDefaultFocus(); // set focus on this item by default
//            }
//        }
//        ImGui::EndCombo();
//    }
//    ImGui::PopItemWidth();
//
////    ImGui::Text("sample %i midi tick %f seconds%f",g_transport.sample_count,g_transport.midi_tick_count,g_transport.current_seconds);
//
//    ImGui::SliderFloat("width", &ticks_per_colum, 0.25f, 32);
////     ImGui::SameLine();
//    ImGui::SliderFloat("height", &note_height, 1.f, 32);
//
//    ImGui::End();
//
//    // ImGui::Begin("sidetoolbar",isOpen,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_HorizontalScrollbar);
//    // ImGui::VSliderFloat("height",ImVec2(16,200),&noteHeight,1.f,16);
//    // ImGui::End();
//
//    ImGui::Begin("piano roll", is_open, ImGuiWindowFlags_HorizontalScrollbar);
//
////    static ImGuiAxis toolbar1_axis = ImGuiAxis_X;
////    DockingToolbar("Toolbar1", &toolbar1_axis);
////    static ImGuiAxis toolbar2_axis = ImGuiAxis_Y;
////    DockingToolbar("Toolbar2", &toolbar2_axis);
//    // const ImVec2 p = ImGui::GetCursorScreenPos();
//
////    print("is window focus %i", ImGui::IsWindowFocused());
//
//    ImDrawList *draw_list = ImGui::GetWindowDrawList();
//
////    const ImVec2 wp = ImGui::GetWindowPos();
//
//    // print(p.x,p.y);
//    ImGuiIO &io = ImGui::GetIO();
//    ImGuiStyle &style = ImGui::GetStyle();
//
//
//    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
//    ImVec2 vMax = ImGui::GetWindowContentRegionMax();
//
//    vMin.x += ImGui::GetWindowPos().x + ImGui::GetScrollX();
//    vMin.y += ImGui::GetWindowPos().y + ImGui::GetScrollY();
//    vMax.x += ImGui::GetWindowPos().x + ImGui::GetScrollX();
//    vMax.y += ImGui::GetWindowPos().y + ImGui::GetScrollY();
//
////    print("vMin",vMin.x,vMin.y,"vMax",vMax.x,vMax.y);
////    print("vMaxX-vMinX",vMax.x-vMin.x);
//    float content_pane_x = vMax.x-vMin.x;
//
//    ImVec2 relative_win_pos = ImGui::GetWindowPos();
//    relative_win_pos.x -= ImGui::GetScrollX();
//    relative_win_pos.y -= ImGui::GetScrollY();
//
//    ImVec2 mouse_pos = ImGui::GetMousePos();
//    // print(mouse_pos.x,mouse_pos.y-relativeWindowPos.y);
//
//    ImVec2 relative_mouse_pos;
//    relative_mouse_pos = mouse_pos - relative_win_pos;
//
//    // grid grids
////    print(ImGui::GetWindowContentRegionMax().x+ImGui::GetScrollX(),ImGui::GetContentRegionMax().y);
//    float space_between_grids = (grid_sz / ticks_per_colum);
//    int grid_loop_size = (content_pane_x);
////    grid_loop_size = grid_loop_size;
////    print("grid loop sz", grid_loop_size);
//
//    for (int i = 0; i < 100; i++)
//    {
//        float x = space_between_grids * i;
//        draw_list->AddLine(
//            ImVec2(relative_win_pos.x + x + 32, relative_win_pos.y + 0),
//            ImVec2(relative_win_pos.x + x + 32,
//            relative_win_pos.y + ImGui::GetWindowHeight()+ImGui::GetScrollY()),
//            ImColor(100, 100, 100, 50)
//        );
//    }
//
//    // draw playhead
//    {
//        float x = g_transport.midi_tick_count * ticks_per_colum;
////        ImGui::SetScrollX(x);
//
//        draw_list->AddLine(
//                           ImVec2(relative_win_pos.x + x + 32, relative_win_pos.y + 0),
//                           ImVec2(relative_win_pos.x + x + 32,
//                                  relative_win_pos.y + ImGui::GetWindowHeight()+ImGui::GetScrollY()),
//                           ImColor(200, 245, 66, 255)
//                           );
//    }
//
////    print("ticks",g_transport.midi_tick_count);
//
//    // draw all notes
//    for(int i = 0; i < midi_track.size(); i++)
//    {
//        // print(midiTrack[i].tick);
//        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(255,0,0,255));
//        ImGui::PopStyleColor();
//
//        midi_note_message &event = midi_track[i];
//
//        if(event.is_note_on)
//        {
////             print("pitch",event->pitch_bend);
//
//            float duration = event.duration;
////            print(duration);
//            float tick = event.tick;
//            float key = event.key;
//            // set up note rectangle dimensions
//            float note_w = duration / ticks_per_colum;
//            float note_x = (tick / ticks_per_colum) + 32;
//            // int noteRange = track.maxNote - track.minNote;
//            // // float note_y = ((noteRange) - (track.notes[i].key - track.minNote)) * noteHeight;
//            float note_y = (127-key) * note_height;
//
//
//            // smf::MidiEvent *endNote = track.notes[i].endNote;
//            int startX = ((float)event.tick / ticks_per_colum) + 32;
//            int endX = ((float)event.tick / ticks_per_colum) + 32 + note_w - 8;
//
//            // print(dur);
//            ImVec2 startbtnsize = ImVec2(8, note_height);
//            ImVec2 startbtnpos = ImVec2(startX, note_y);
//            ImVec2 endbtnpos = ImVec2(endX, note_y);
//
//            ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(255,0,0,255));
//
//            ImGui::SetCursorPos(startbtnpos);
//            ImGui::Button(std::to_string(i).c_str(),startbtnsize);
//            if (ImGui::IsItemClicked())
//            {
//                 print("start note",i);
//                note_left_drag_idx = i;
////                note_idx=i;
//                last_event_clicked = midi_track[note_left_drag_idx];
//            }
//            ImGui::SetCursorPos(endbtnpos);
//            ImGui::Button(std::to_string(i).c_str(),startbtnsize);
//            // selects note
//            if (ImGui::IsItemClicked())
//            {
//                // print("start??",i);
//                note_right_drag_idx = i;
////                note_idx=i;
//                last_event_clicked = *midi_track[note_right_drag_idx].linked_event;
//            }
//            ImGui::PopStyleColor();
//
//            ImVec2 size = ImVec2(note_w, note_height);
//            ImGui::SetCursorPos(ImVec2(note_x,note_y));
//            ImVec2 button_size(note_w, note_height);
//            ImGui::PushID(i);
//            // ImGui::PushStyleColor(ImGuiCol_Button,key_color);
//            ImGui::Button(" ",button_size);
//            // ImGui::PopStyleColor();
//            ImGui::PopID();
//
////            ImVec2 rect_min = relative_win_pos+ImVec2(note_x, note_y); // Adjust the values here to position the rectangle
////            ImVec2 rect_max = relative_win_pos+ImVec2(note_x+note_w, note_y+noteHeight); // Adjust the values here to position the rectangle
////
////            ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //RGBA color values ranging from 0 to 1
////
////            draw_list->AddRect(rect_min, rect_max, ImGui::GetColorU32(color));
//
//            // draw button
//            ImVec2 rect_min = relative_win_pos + ImVec2(note_x, note_y); // Adjust the values here to position the rectangle
//            ImVec2 rect_max = relative_win_pos + ImVec2(note_x + note_w, note_y + note_height); // Adjust the values here to position the rectangle
//            ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 0.35f); //RGBA color values ranging from 0 to 1
//
//            // Define the vertices of the rectangle
//            ImVec2 vertices[4];
//            vertices[0] = ImVec2(rect_min.x, rect_min.y-event.pitch_bend_a);
//            vertices[1] = ImVec2(rect_max.x, rect_min.y-event.pitch_bend_b);
//            vertices[2] = ImVec2(rect_max.x, rect_max.y-event.pitch_bend_b);
//            vertices[3] = ImVec2(rect_min.x, rect_max.y-event.pitch_bend_a);
//
////             Draw the filled rectangle
//            draw_list->AddConvexPolyFilled(vertices, 4, ImGui::GetColorU32(color));
//
//
//            draw_list->AddLine(
//                               relative_win_pos+ImVec2(note_x,note_y-event.pitch_bend_a+(note_height/2)),
//                               relative_win_pos+ImVec2(note_x+note_w,note_y-event.pitch_bend_b+(note_height/2)),
//                                   ImGui::GetColorU32(ImVec4(1,0.1,1,1))
//                               );
//
//            // selects note
//            if (ImGui::IsItemClicked())
//            {
//                note_idx = i;
//                // mynote = track.notes[sel];
//                last_event_clicked = midi_track[note_idx];
////                print("sel",sel);
//            }
//            //double click remove note;
//            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && note_idx!=-1)
//            {
////                midi_track[note_idx].clear();
//                midi_track.erase(midi_track.begin() + note_idx);
//
////                midiTrack.removeEmpties();
//            }
//        };
//
//        // handles moving note
//        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
//        {
//            // print("sel", sel, " ", ImGui::GetMouseDragDelta().x, " ", ImGui::GetMouseDragDelta().y);
//            ImVec2 new_pos = ImGui::GetMouseDragDelta();
//            // points[i].x = pos.x;
//
//            // main selection
//            if (note_idx != -1)
//            {
//                // ((float)(track.tpq / ticksPerColum)/4)
//                int offset = (new_pos.x * ticks_per_colum);
//                int dur = midi_track[note_idx].get_duration();
//                int s = snap(last_event_clicked.tick + offset,grid_sz);
//                midi_track[note_idx].tick = s;
//
//                // move other note
////                print("?",midi_track[note_idx].linked_event);
//                midi_track[note_idx].linked_event->tick = dur + s;
//                // print(midiTrack[sel].tick,midiTrack[sel].getLinkedEvent()->tick);
//
//                int new_key = last_event_clicked.key + (round(new_pos.y / note_height) * -1);
//                midi_track[note_idx].key = new_key;
//                midi_track[note_idx].linked_event->key=new_key;
//
//            } else {
//
////                int offset = (new_pos.x * ticksPerColum);
////                int left_tick = midiTrack[noteleft_dragIdx].tick;
////                int right_tick = midiTrack[noteright_dragIdx].tick;
//
//                bool shift_down = ImGui::GetIO().KeyShift;
//
//                print("main",note_idx,"left",note_left_drag_idx,"right",note_right_drag_idx);
//
//                // begin note button
//                if(note_left_drag_idx != -1)
//                {
////                    print("new drag pos left",new_pos.x,new_pos.y);
//                    int offset = (new_pos.x * ticks_per_colum);
//
//                    int left_tick = snap(last_event_clicked.tick + offset, grid_sz);
//                    int right_tick = midi_track[note_left_drag_idx].linked_event->tick;
////                    print("lr",left_tick,right_tick);
//
//                    if(left_tick+grid_sz-1<right_tick)
//                    {
//                        midi_track[note_left_drag_idx].tick = left_tick;
//                        midi_track[note_left_drag_idx].set_duration();
////                        midi_track[note_left_drag_idx].duration = midi_track[note_left_drag_idx].get_duration();
////                        print("dur",midi_track[note_left_drag_idx].get_duration());
////                        midi_track[note_left_drag_idx].set_duration();
////                        print("duration");
////                        print("changed tick", midi_track[note_left_drag_idx].tick);
//                    }
//
//                    if (shift_down) {
//                        midi_track[note_left_drag_idx].pitch_bend_a = snap(-new_pos.y,note_height);
//                    }
//                }
//
//                // end note button
//                if(note_right_drag_idx != -1)
//                {
////                    print("new drag pos right",new_pos.x,new_pos.y);
//                    int offset = (new_pos.x * ticks_per_colum);
//
//                    int left_tick = midi_track[note_right_drag_idx].tick;
//                    int right_tick = snap(last_event_clicked.tick + offset, grid_sz);
//                    // print(s);
//
////                    print("left",left_tick,"right",right_tick);
//
//                    // make sure right button doesnt go farther back than left
//                    if(left_tick<right_tick)
//                    {
//                        midi_track[note_right_drag_idx].linked_event->tick = right_tick;
//                        midi_track[note_right_drag_idx].set_duration();
////                        print("changed tick", midi_track[note_right_drag_idx].linked_event->tick,right_tick);
//                    }
//
//                    if (shift_down) {
//                        midi_track[note_right_drag_idx].pitch_bend_b = snap(-new_pos.y,note_height);
//                    }
//                }
//
////                print("note left idx",noteleft_dragIdx, "note right idx",noteright_dragIdx);
//
//            }
//
//        }
//    }
//
//    // float hey = (96.f / 4);
//
//    // draw piano buttons
//    for(int i = 0; i < 128; i++)
//    {
//        int i2 = 127-i;
//        int bw = piano_keys[i%12];
//        uint32_t key_color = ImColor(bw,bw,bw);
//
//        ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX(),note_height*i2));
//        ImVec2 button_size(32, note_height);
//        ImGui::PushID(i);
//        ImGui::PushStyleColor(ImGuiCol_Button,key_color);
//        ImGui::Button(" ",button_size);
//        // if(ImGui::IsItemHovered())
//        // {
//        //     // ImGui::PushStyleColor(ImGuiCol_Button,ImColor(255,0,255));
//        //     print("hovering...");
//        //     ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX()+32,noteHeight*i2));
//        //     ImGui::Text(std::to_string(i).c_str());
//        // }
//        ImGui::PopStyleColor();
//        ImGui::PopID();
//
////         draw_list->AddRectFilled(
////                 ImVec2(ImGui::GetWindowPos().x,
////             relativeWindowPos.y + (i*noteHeight)),
////                 ImVec2(ImGui::GetWindowPos().x + 32,
////             relativeWindowPos.y + noteHeight + (i*noteHeight)),
////             key_color, 1.0f,
////             ImDrawCornerFlags_All
////         );
//
//    }
//
//    if(ImGui::IsWindowHovered())
//    {
//        // note debuggggg
//        float note_snap_y = floor(relative_mouse_pos.y/note_height);
//        int notenum = 127-int(note_snap_y);
//        current_hovered_note_num = notenum;
//        int octave = int(notenum / 12) - 1;
//        int note = (notenum % 12);
//        // print(note_names[note]);
//        if(note>-1) // so it doesnt draw past window area
//        {
//            ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX()+38,note_snap_y*note_height));
//        }
////        print("note",note_names[note].c_str(),note);
//        ImGui::Text("%s %i", note_names[note].c_str(), notenum);
//    }
//
//    // resets note selection
//    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
//    {
////        print("ImGuiMouseButton_Left released");
//        note_idx = -1;
//        note_left_drag_idx = -1;
//        note_right_drag_idx = -1;
//    }
//
//    if(ImGui::IsWindowFocused() && mouse_pos.y<vMax.y && mouse_pos.x<vMax.x && mouse_pos.y>vMin.y && mouse_pos.x>vMin.x) // need focus to insert note and not clicking outside scrollbar bounds
//    {
//        // insert note if nothing is selected and clicked
//        if(note_idx==-1&&note_left_drag_idx==-1&&note_right_drag_idx==-1)
//        {
//            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
//            {
//
////                int key = (floor(relative_mouse_pos.y/note_height)*-1) - 1; // why is this negative?
//                int key = current_hovered_note_num;
//                print(key);
//
////                int key = 60;
//                int tick = (mouse_pos.x-32+ImGui::GetScrollX()-ImGui::GetWindowPos().x) * ticks_per_colum;
//
//                //             tick = snap(tick,divsize);
//                tick = floor(tick/grid_sz)*grid_sz;
//
//                midi_note_message event_on;
//
//                event_on.tick = tick;
//                event_on.velocity=60;
//                event_on.key = key;
//                event_on.is_note_on = true;
//                event_on.duration=(grid_sz*1);
//                midi_track.push_back(event_on);
//
////                midi_note_message *event_on_ptr = &midi_track.back();
////                print(event_on_ptr->velocity,"i can read just fine");
//
//                midi_note_message event_off;
//                event_off.tick = tick+(grid_sz*1);
//                event_off.velocity = 0;
//                event_off.key = key;
//                event_off.is_note_on = false;
//                event_off.duration=(grid_sz*1);
//
//                midi_track.push_back(event_off);
//
////                midi_note_message *event_off_ptr = &midi_track.back();
//
//                midi_note_message *event_on_ptr = &midi_track[midi_track.size()-2];
//                midi_note_message *event_off_ptr = &midi_track[midi_track.size()-1];
//
//                event_on_ptr->linked_event=event_off_ptr;
//                event_off_ptr->linked_event=event_on_ptr;
//
//
//
////                print("event on ptr, now i cant read from it",event_on_ptr->velocity,"event off ptr",event_off_ptr->velocity);
//
////                event_off_ptr->duration=(grid_sz*1);
////                event_on_ptr->duration=(grid_sz*1);
////                event_off_ptr->duration=(grid_sz*1);
////                midi_file.addEvent(eventOff);
//
////                midi_file.linkNotePairs();
//
//                // event.makeNoteOff();
//                // midiFile.addEvent(event);
//                print("insert note",key,event_on_ptr->duration,mouse_pos.x,mouse_pos.y,tick);
//            }
//        }
//    }
//
////    draw_list->AddRect( vMin, vMax, IM_COL32( 255, 0, 0, 255 ) );
//
//    ImGui::End();
//}

void sequencer_controls_window()
{
    static bool is_sequencer_controls_window_open = true;
    ImGui::Begin("sequencer controls", &is_sequencer_controls_window_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);


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

    ImGui::PopItemWidth();

    ImGui::SameLine();

    ImGui::Text("sample %i midi tick %f seconds%f",g_transport.sample_count,g_transport.midi_tick_count,g_transport.current_seconds);

//    ImGui::SliderFloat("width", &ticks_per_colum, 0.25f, 32);
////     ImGui::SameLine();
//    ImGui::SliderFloat("height", &note_height, 1.f, 32);

    ImGui::End();
}

void piano_roll_window(bool *is_open, smf::MidiFile& midi_file)
{
//    smf::MidiFile& midi_file = p_midi_file;
    smf::MidiEventList& midi_track = midi_file[0];

    static int note_idx = -1; // track note index??
    static int note_left_drag_idx = -1;
    static int note_right_drag_idx = -1;
    static smf::MidiEvent last_event_clicked;
    static float ticks_per_colum = 1;
    static float note_height = 8;
    static int grid_div = 4;
    static float grid_sz = (96.f / grid_div);

    
    ImGui::Begin("fix me", is_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);

//    ImGui::SameLine();

    static int selected_item = 0;
    const char* items[] = { "1", "2", "3" };
    const int num_items = sizeof(items) / sizeof(items[0]);

    ImGui::PushItemWidth(40.0f);
    if (ImGui::BeginCombo("##div", items[selected_item])) {
        for (int i = 0; i < num_items; ++i) {
            const bool is_selected = (selected_item == i);
            if (ImGui::Selectable(items[i], is_selected)) {
                selected_item = i;
//                print(i);
                if(i==0)
                {
                    grid_div = 4;
                    grid_sz = (96.f / grid_div);
                }
                if(i==1)
                {
                    grid_div = 8;
                    grid_sz = (96.f / grid_div);
                }

            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus(); // set focus on this item by default
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::Text("sample %i midi tick %f seconds%f",g_transport.sample_count,g_transport.midi_tick_count,g_transport.current_seconds);

    ImGui::SliderFloat("width", &ticks_per_colum, 0.25f, 32);
//     ImGui::SameLine();
    ImGui::SliderFloat("height", &note_height, 1.f, 32);

    ImGui::End();

//     ImGui::Begin("sidetoolbar",isOpen,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_HorizontalScrollbar);
//     ImGui::VSliderFloat("height",ImVec2(16,200),&noteHeight,1.f,16);
//     ImGui::End();

//    sprintf_l("p %s", node_id)
//    char piano_roll_string[64];
//    snprintf(piano_roll_string, sizeof(piano_roll_string), "piano roll (%d)", node_id);

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

    ImVec2 relative_mouse_pos;
    relative_mouse_pos = mouse_pos - relative_win_pos;

    // grid grids
//    print(ImGui::GetWindowContentRegionMax().x+ImGui::GetScrollX(),ImGui::GetContentRegionMax().y);
    float space_between_grids = (grid_sz / ticks_per_colum);
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
    
//    g_transport.current_pattern
//    int ticks_per_pattern = 96*4;
//    print(g_transport.midi_tick_count/(ticks_per_pattern));
//    print(g_transport.current_pattern_open);
//    print(g_transport.current_pattern,g_transport.current_column_open);

//    if(g_transport.current_pattern==g_transport.current_column_open)
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
    for(int i = 0; i < midi_track.size(); i++)
    {
        // print(midiTrack[i].tick);
        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(255,0,0,255));
        ImGui::PopStyleColor();

        smf::MidiEvent *event = &midi_track[i];
        
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
                note_left_drag_idx = i;
                last_event_clicked = midi_track[note_left_drag_idx];
            }
            ImGui::SetCursorPos(endbtnpos);
            ImGui::Button(std::to_string(i).c_str(),startbtnsize);
            // selects note
            if (ImGui::IsItemClicked())
            {
                // print("start??",i);
                note_right_drag_idx = i;
                last_event_clicked = *midi_track[note_right_drag_idx].getLinkedEvent();
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

//            // Define the vertices of the rectangle
//            ImVec2 vertices[4];
//            vertices[0] = ImVec2(rect_min.x, rect_min.y-event->pitch_bend_a);
//            vertices[1] = ImVec2(rect_max.x, rect_min.y-event->pitch_bend_b);
//            vertices[2] = ImVec2(rect_max.x, rect_max.y-event->pitch_bend_b);
//            vertices[3] = ImVec2(rect_min.x, rect_max.y-event->pitch_bend_a);
//
////             Draw the filled rectangle
//            draw_list->AddConvexPolyFilled(vertices, 4, ImGui::GetColorU32(color));
            
            
            draw_list->AddLine(
                                   relative_win_pos+ImVec2(note_x,note_y-event->pitch_bend_a*note_height+(note_height/2)),
                                   relative_win_pos+ImVec2(note_x+note_w,note_y-event->pitch_bend_b*note_height+(note_height/2)),
                                   ImGui::GetColorU32(ImVec4(1,0.1,1,1))
                               );

            // selects note
            if (ImGui::IsItemClicked())
            {
                note_idx = i;
                // mynote = track.notes[sel];
                last_event_clicked = midi_track[note_idx];
//                print("sel",sel);
            }
            //double click remove note;
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && note_idx!=-1)
            {
//                print("num events",midi_track.size());
                midi_track[note_idx].getLinkedEvent()->clear();
                midi_track[note_idx].clear();
//                midi_track.erase(note_idx);
                midi_track.removeEmpties();
//
//                print("deleted note");
//                print("num events",midi_track.size());
            }
        };

        // handles moving note
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            // print("sel", sel, " ", ImGui::GetMouseDragDelta().x, " ", ImGui::GetMouseDragDelta().y);
            ImVec2 new_pos = ImGui::GetMouseDragDelta();
            // points[i].x = pos.x;
            
            // main selection
            if (note_idx != -1)
            {
                
                bool shift_down = ImGui::GetIO().KeyShift;
                if(shift_down)
                {
                    print("shift down main");
                    print(snap(-new_pos.y,note_height)/note_height);
                } else {
                    // ((float)(track.tpq / ticksPerColum)/4)
                    int offset = (new_pos.x * ticks_per_colum);
                    int dur = midi_track[note_idx].getTickDuration();
                    int s = snap(last_event_clicked.tick + offset,grid_sz);
                    midi_track[note_idx].tick = s;
                    
                    // move other note
                    midi_track[note_idx].getLinkedEvent()->tick = dur + s;
                    // print(midiTrack[sel].tick,midiTrack[sel].getLinkedEvent()->tick);
                    
                    int new_key = last_event_clicked.getKeyNumber() + (round(new_pos.y / note_height) * -1);
                    midi_track[note_idx].setKeyNumber(new_key);
                    midi_track[note_idx].getLinkedEvent()->setKeyNumber(new_key);
                }


            } else {
                
//                int offset = (new_pos.x * ticksPerColum);
//                int left_tick = midiTrack[noteleft_dragIdx].tick;
//                int right_tick = midiTrack[noteright_dragIdx].tick;

                bool shift_down = ImGui::GetIO().KeyShift;

                // begin note button
                if(note_left_drag_idx != -1)
                {
//                    print("new drag pos left",new_pos.x,new_pos.y);
                    int offset = (new_pos.x * ticks_per_colum);
                    
                    int left_tick = snap(last_event_clicked.tick + offset, grid_sz);
                    int right_tick = midi_track[note_left_drag_idx].getLinkedEvent()->tick;
//                    print("lr",left_tick,right_tick);

                    if(left_tick+grid_sz-1<right_tick)
                    {
                        midi_track[note_left_drag_idx].tick = left_tick;
                    }
                    
                    if (shift_down) {
                        midi_track[note_left_drag_idx].pitch_bend_a = snap(-new_pos.y,note_height)/note_height;
//                        midi_track[note_left_drag_idx].getLinkedEvent()->pitch_bend_a = snap(-new_pos.y,note_height);
                    }
                }
                
                // end note button
                if(note_right_drag_idx != -1)
                {
//                    print("new drag pos right",new_pos.x,new_pos.y);
                    int offset = (new_pos.x * ticks_per_colum);
                    
                    int left_tick = midi_track[note_right_drag_idx].tick;
                    int right_tick = snap(last_event_clicked.tick + offset, grid_sz);
                    // print(s);
                    
//                    print("left",left_tick,"right",right_tick);
                    
                    // make sure right button doesnt go farther back than left
                    if(left_tick<right_tick)
                    {
                        midi_track[note_right_drag_idx].getLinkedEvent()->tick = right_tick-1;//fixme sometimes the next note doesnt play
                    }
                    
                    if (shift_down) {
                        midi_track[note_right_drag_idx].pitch_bend_b = snap(-new_pos.y,note_height)/note_height;
//                        midi_track[note_right_drag_idx].getLinkedEvent()->pitch_bend_b = snap(-new_pos.y,note_height);
                    }
                }
                
//                print("note left idx",noteleft_dragIdx, "note right idx",noteright_dragIdx);
                
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
        note_idx = -1;
        note_left_drag_idx = -1;
        note_right_drag_idx = -1;
    }
    
    if(ImGui::IsWindowFocused() && mouse_pos.y<vMax.y && mouse_pos.x<vMax.x && mouse_pos.y>vMin.y && mouse_pos.x>vMin.x) // need focus to insert note and not clicking outside scrollbar bounds
    {
        // insert note if nothing is selected and clicked
        if(note_idx==-1&&note_left_drag_idx==-1&&note_right_drag_idx==-1)
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                
                int key = (floor(relative_mouse_pos.y/note_height)*-1) - 1; // why is this negative?
                print(key);
                
                int tick = (mouse_pos.x-32+ImGui::GetScrollX()-ImGui::GetWindowPos().x) * ticks_per_colum;
                
                //             tick = snap(tick,divsize);
                tick = floor(tick/grid_sz)*grid_sz;
                
                smf::MidiEvent eventOn;
                eventOn.tick = tick;
                eventOn.track = 0;
                eventOn.makeNoteOn(0, key, 60);
                midi_file.addEvent(eventOn);
                
                smf::MidiEvent eventOff;
                eventOff.tick = tick+(grid_sz*1);
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
