#pragma once

#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

class WindowManager
{
private:
    GLFWwindow* window = nullptr;

public:
    // methods
    ~WindowManager();
    bool Init(int width, int height);
    void ResetWindow();
    void UpdateWindow();
    bool ShouldClose();
    void Destroy();
};
