#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "imnodes.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>

// idea taken from codetechandtutorials/imGUIexample on github, applied to to SDL backend
// usage: make an abstract class inherenting this and override the Update() function with your gui code
struct im_wrap
{
//public:
    void init(SDL_Window* window, SDL_GLContext context);
    void new_frame();
    void update();
    void shutdown();
    void render();
    SDL_Window* window;
    ImFont* font;
    ImGuiIO* io;
    ImGuiContext* imgui_ctx;
    ImNodesContext* imnode_ctx;
};
