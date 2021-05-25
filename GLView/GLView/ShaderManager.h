#pragma once
#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

class ShaderManager
{
private:
    GLuint shaderProgram;
    GLuint CompileShader(GLenum type, char* code);

public:
    void Destroy();
    bool Load();
    void Use();
};
