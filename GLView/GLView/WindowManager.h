#pragma once

#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "vectors.h"

class WindowManager
{
    static WindowManager* current;
private:
    GLFWwindow* window = nullptr;
    int width = 0;
    int height = 0;

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
