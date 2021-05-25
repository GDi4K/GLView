#include "ShaderManager.h"

char vertexShaderCode[] = ""
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "void main() {\n"
        "   gl_Position = vec4(pos, 1.0);\n"
        "};\n"
        "";

char fragmentShaderCode[] = ""
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   vec3 color = vec3(1.0);\n"
        "   FragColor = vec4(color, 1.0);\n"
        "}\n"
        "";

GLuint ShaderManager::CompileShader(GLenum type, char* code)
{
    auto shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compilation failed: " << log << std::endl;
        return 0;
    }

    return shader;
}

bool ShaderManager::Load()
{
    const auto vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderCode);
    const auto fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);
    if (!vertexShader || !fragmentShader)
    {
        return false;
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(shaderProgram, sizeof(log), nullptr, log);
        std::cerr << "Shader linking failed: " << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void ShaderManager::Use()
{
    glUseProgram(shaderProgram);
}

void ShaderManager::Destroy()
{
    glDeleteShader(shaderProgram);
    shaderProgram = NULL;
}