#include "ShaderManager.h"

int StringToWString(std::wstring &ws, const std::string &s)
{
    std::wstring wsTmp(s.begin(), s.end());

    ws = wsTmp;

    return 0;
}

ShaderManager::ShaderManager()
{
    vertexShaderCode = ""
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "void main() {\n"
        "   gl_Position = vec4(pos, 1.0);\n"
        "};\n"
        "";

    fragmentShaderCode = ""
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   vec3 color = vec3(1.0);\n"
        "   FragColor = vec4(color, 1.0);\n"
        "}\n"
        "";
}

GLuint ShaderManager::CompileShader(GLenum type, std::string code)
{
    const char* ccode = code.c_str();
    auto shader = glCreateShader(type);
    glShaderSource(shader, 1, &ccode, nullptr);
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

bool ShaderManager::Compile()
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

void ShaderManager::Recompile()
{
    if (recompiling) return;
    recompiling = true;
    
    std::cout << "Recompiling" << std::endl;
    
    if (!vertexShaderCodePath.empty())
    {
        LoadVertexShader(vertexShaderCodePath);
    }

    if(!fragmentShaderCodePath.empty())
    {
        LoadFragmentShader(fragmentShaderCodePath);
    }
    
    glDeleteProgram(shaderProgram);
    Compile();
    
    recompiling = false;
}

void ShaderManager::Use()
{
    glUseProgram(shaderProgram);
}

bool ShaderManager::ReadFile(std::string path, std::string& content)
{
    std::ifstream file(path);
    std::string line;
    
    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            content += line + "\n";
        }
    } else
    {
        return false;
    }

    file.close();
    
    return true;
}

bool ShaderManager::LoadVertexShader(std::string path)
{
    vertexShaderCodePath = path;
    std::cout << "Loading vertex shader at: " << path << std::endl;
    vertexShaderCode = "";

    if(!ReadFile(path, vertexShaderCode))
    {
        std::cerr << "Reading shader file failed!" << std::endl;
        return false;
    }
    
    return true;
}

bool ShaderManager::LoadFragmentShader(std::string path)
{
    fragmentShaderCodePath = path;
    std::cout << "Loading fragment shader at: " << path << std::endl;
    fragmentShaderCode = "";

    if(!ReadFile(path, fragmentShaderCode))
    {
        std::cerr << "Reading shader file failed!" << std::endl;
        return false;
    }

    return true;
}

bool ShaderManager::SetUniformInt(std::string key, int value)
{
    const auto location = glGetUniformLocation(shaderProgram, key.c_str());
    if (location == -1) return false;
    
    glUniform1i(location, value);
    return true;
}

bool ShaderManager::SetUniformFloat(std::string key, float value)
{
    const auto location = glGetUniformLocation(shaderProgram, key.c_str());
    if (location == -1) return false;
    
    glUniform1f(location, value);
    return true;
}

bool ShaderManager::SetUniformFloat2(std::string key, float v1, float v2)
{
    const auto location = glGetUniformLocation(shaderProgram, key.c_str());
    if (location == -1) return false;
    
    glUniform2f(location, v1, v2);
    return true;
}

bool ShaderManager::SetUniformFloat3(std::string key, float v1, float v2, float v3)
{
    const auto location = glGetUniformLocation(shaderProgram, key.c_str());
    if (location == -1) return false;
    
    glUniform3f(location, v1, v2, v3);
    return true;
}

bool ShaderManager::SetUniformFloat4(std::string key, float v1, float v2, float v3, float v4)
{
    const auto location = glGetUniformLocation(shaderProgram, key.c_str());
    if (location == -1) return false;
    
    glUniform4f(location, v1, v2, v3, v4);
    return true;
}

void ShaderManager::Destroy()
{
    glDeleteShader(shaderProgram);
    shaderProgram = NULL;
}
