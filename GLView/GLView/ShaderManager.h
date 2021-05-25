#pragma once
#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

class ShaderManager
{
private:
    GLuint shaderProgram;
    GLuint CompileShader(GLenum type, std::string code);
    std::string vertexShaderCode;
    std::string fragmentShaderCode;

public:
    ShaderManager();
    void Destroy();
    bool Load();
    void Use();
};
