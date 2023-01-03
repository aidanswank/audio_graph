#include "im_wrap.h"
#include "vprint.h"

void im_wrap::init(SDL_Window* p_Window, SDL_GLContext p_Context)
{ 
    Window = p_Window;

    IMGUI_CHECKVERSION();
    imgui_ctx = ImGui::CreateContext();
//    imnode_ctx = ImNodes::CreateContext(); // node graph
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
//    ImGui::StyleColorsClassic();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(Window, p_Context);
    ImGui_ImplOpenGL3_Init("#version 330 core"); // glsl version

    font = io->Fonts->AddFontFromFileTTF("/Users/aidan/dev/cpp/node_editor/res/fonts/unifont-14.0.01.ttf", 16.0f);
    
    ImFontConfig config;
    config.OversampleH = 2;
//    config.OversampleV = 1;
//    config.GlyphExtraSpacing.x = 1.0f;
//    font = io->Fonts->AddFontFromFileTTF("/Users/aidan/Downloads/fragment_mono/fonts/ttf/FragmentMono-Regular.ttf", 32.0f, &config);

    io->IniFilename = "/Users/aidan/imgui.ini";
//    io->FontGlobalScale = 0.5f;
    // std::cout << &Window << std::endl;
}

void im_wrap::new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(Window);
    // std::cout << &Window << std::endl;
    ImGui::NewFrame();
}

void im_wrap::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Swap buffers
    SDL_GL_SwapWindow(Window);

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

}

void im_wrap::update()
{
    ImGui::PushFont(font);

    ImGui::ShowDemoWindow();

    ImGui::PopFont();
}

void im_wrap::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
//    ImNodes::DestroyContext();
}
