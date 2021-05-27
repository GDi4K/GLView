#pragma once

#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "vectors.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imfilebrowser.h"
#include "ShaderManager.h"

class WindowManager
{
    static WindowManager* current;
private:
    GLFWwindow* window = nullptr;
    int width = 0;
    int height = 0;
    bool imguiUpdated = false;
    ImGui::FileBrowser fileDialog;
    bool startSelectingShaders = false;
    bool isVertexShaderSelected = false;

public:
    // methods
    ~WindowManager();
    bool Init(int width, int height);
    void ResetWindow();
    void UpdateWindow();
    bool ShouldClose();
    void Destroy();
    void ResizeWindow(int width, int height);
    bool ShouldReload();
    Float2 GetMousePosition();
    void InitImGui();
    void ShowError(std::string title, std::string message);
    void SelectShader(ShaderManager &shaderManager);
    bool ShouldSelectShaders();

    // getters
    Int2 GetResolution()
    {
        return { width, height };
    }

    // static methods
    static WindowManager* Get();

    // callbacks
    static void OnResizeWindow(GLFWwindow* window, int width, int height);
    
};
