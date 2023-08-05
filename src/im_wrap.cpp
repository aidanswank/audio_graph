#include "im_wrap.h"
#include "vprint.h"
#include "resource.h"

void im_wrap::init(SDL_Window* p_window, SDL_GLContext p_context)
{
    window = p_window;

    IMGUI_CHECKVERSION();
    imgui_ctx = ImGui::CreateContext();
    imnode_ctx = ImNodes::CreateContext(); // node graph
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
//    ImGui::StyleColorsClassic();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, p_context);
    ImGui_ImplOpenGL3_Init("#version 330 core"); // glsl version

    font = io->Fonts->AddFontFromFileTTF(getResourcePath("unifont-14.0.01.ttf").c_str(), 16.0f);
    
    ImFontConfig config;
//    config.OversampleH = 2;
//    config.OversampleV = 1;
//    config.GlyphExtraSpacing.x = 1.0f;
//    font = io->Fonts->AddFontFromFileTTF("/Users/aidan/Downloads/fragment_mono/fonts/ttf/FragmentMono-Regular.ttf", 32.0f, &config);
//    io->IniFilename = "imgui.ini";
    
    // Assuming "my_resource.txt" is in your app's bundle.
    std::string resourceFileName = "imgui.ini";
    
    // Get the resource path using the getResourcePath function.
    std::string resourcePath = getResourcePath(resourceFileName);

//    if (!resourcePath.empty()) {
//        // Resource found, let's read and print its contents.
//        std::ifstream file(resourcePath);
//        if (file.is_open()) {
//            std::string line;
//            while (std::getline(file, line)) {
//                std::cout << line << std::endl;
//            }
//            file.close();
//        } else {
//            std::cerr << "Error opening resource file: " << resourceFileName << std::endl;
//        }
//    } else {
//        std::cerr << "Resource not found: " << resourceFileName << std::endl;
//    }
    
//    if (!resourcePath.empty()) {
//        io->IniFilename = resourcePath.c_str();
//    } else {
//        std::cerr << "Resource not found: " << resourceFileName << std::endl;
//    }
//    io->IniFilename = "/Users/aidan/dev/cpp/dfs_modules/build/Debug/audio_graph.app/Contents/Resources/imgui.ini";
//    io->IniFilename = getResourcePath("imgui.ini").c_str();
    ImGui::LoadIniSettingsFromDisk(getResourcePath("imgui.ini").c_str());
    print("filepath",io->IniFilename);
//    io->FontGlobalScale = 0.5f;
    // std::cout << &Window << std::endl;
}

void im_wrap::new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    // std::cout << &Window << std::endl;
    ImGui::NewFrame();
}

void im_wrap::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Swap buffers
    SDL_GL_SwapWindow(window);

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
    ImNodes::DestroyContext();
}
