#include "ShaderManager.h"
#include <regex>
#include "cwalk.h"
#include "Texture.h"

int StringToWString(std::wstring& ws, const std::string& s)
{
    std::wstring wsTmp(s.begin(), s.end());

    ws = wsTmp;

    return 0;
}

ShaderManager::ShaderManager()
{
    vertexShaderCode = ""
        "#version 330 core\n"
        "layout (location = 0) in vec3 aVertexPos;\n"
        "layout (location = 1) in vec2 aTextCoords;\n"
        ""
        "out vec3 vertexPos;\n"
        "out vec2 textCoords;\n"
        ""
        "void main() {\n"
        "   vertexPos = aVertexPos;\n"
        "   textCoords = aTextCoords;\n"
        "   gl_Position = vec4(aVertexPos, 1.0);\n"
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
        compilerErrors += std::string(log) + "\n";
        std::cerr << "Shader compilation failed: " << log << std::endl;
        return 0;
    }

    return shader;
}

bool ShaderManager::Compile()
{
    const auto vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderCode);
    const auto fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);
    if (vertexShader == 0 || fragmentShader == 0)
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
        compilerErrors += std::string("Shader linking failed") + "\n";
        std::cerr << "Shader linking failed: " << std::endl;
        return false;
    }

    // load textures if any
    for (int lc = 0; lc < 16; lc++)
    {
        auto textureInfo = &(textures[lc]);

        if (textureInfo->loaded) continue;
        if (!textureInfo->assigned) continue;
        if (!textureInfo->pathAssigned) continue;

        if (!textureInfo->Load())
        {
            compilerErrors += std::string("Failed to load texture at: ") + textureInfo->filePath + "\n";
            std::cout << "Failed to load texture" << std::endl;
            return false;
        }
        
        textureInfo->FreeData();
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void ShaderManager::Recompile()
{
    if (recompiling) return;
    recompiling = true;
    compilerErrors = "";

    compilingFailed = false;
    std::cout << "Recompiling" << std::endl;

    if (!vertexShaderCodePath.empty())
    {
        if (!LoadVertexShader(vertexShaderCodePath))
        {
            compilingFailed = true;
        }
    }

    if (!fragmentShaderCodePath.empty())
    {
        if (!LoadFragmentShader(fragmentShaderCodePath))
        {
            compilingFailed = true;
        }
    }

    glDeleteProgram(shaderProgram);
    if (!Compile())
    {
        compilingFailed = true;
    }

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
            // handling #includes
            auto found = line.find("#include");
            if (found == 0)
            {
                auto includePath = std::regex_replace(line, std::regex("#include"), "");
                includePath = std::regex_replace(includePath, std::regex("[\"' ]*"), "");
                auto resolvedIncludePath = resolveIncludePath(path, includePath);

                std::string includeContent;
                if (!ReadFile(resolvedIncludePath, includeContent))
                {
                    auto message = "Failed to load include file: " + includePath + " inside: " + path;
                    compilerErrors += message + "\n";
                    std::cerr << message << std::endl;
                    return false;
                }

                content += includeContent + "\n";
                continue;
            }

            // handling #texture
            auto foundTexture = line.find("#texture");
            if (foundTexture == 0)
            {
                auto textureInfo = std::regex_replace(line, std::regex("#texture "), "");
                auto textureNumber = stoi(textureInfo.substr(0, 1));
                auto texturePath = textureInfo.substr(2);
                texturePath = std::regex_replace(texturePath, std::regex("[\"']*"), "");
                auto resolvedTexturePath = resolveIncludePath(path, texturePath);

                if (textureNumber < 16)
                {
                    if (textures[textureNumber].filePath.compare(resolvedTexturePath))
                    {
                        textures[textureNumber].filePath = resolvedTexturePath;
                        textures[textureNumber].pathAssigned = true;
                        textures[textureNumber].loaded = false;
                    }
                }
                else
                {
                    auto message = "Invalid texture position on: " + path;
                    compilerErrors += message + "\n";
                    std::cerr << message << std::endl;
                    return false;
                }

                continue;
            }
            content += line + "\n";
        }
    }
    else
    {
        return false;
    }

    file.close();

    return true;
}

std::string ShaderManager::resolveIncludePath(std::string parentPath, std::string includePath)
{
    size_t parentDirPathLength;
    cwk_path_get_dirname(parentPath.c_str(), &parentDirPathLength);
    const std::string parentDirPath = parentPath.substr(0, parentDirPathLength);

    char resolvedPath[1024];
    cwk_path_join(parentDirPath.c_str(), includePath.c_str(), resolvedPath, sizeof(resolvedPath));

    return resolvedPath;
}

bool ShaderManager::LoadVertexShader(std::string path)
{
    vertexShaderCodePath = path;
    std::cout << "Loading vertex shader at: " << path << std::endl;
    vertexShaderCode = "";

    if (!ReadFile(path, vertexShaderCode))
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

    if (!ReadFile(path, fragmentShaderCode))
    {
        std::cerr << "Reading shader file failed!" << std::endl;
        return false;
    }

    return true;
}

void ShaderManager::AssignTextureHooks(GLuint* hooks)
{
    int size = std::max<int>(16, sizeof(hooks) / sizeof(GLuint));
    for (int lc = 0; lc < size; lc++)
    {
        textures[lc].textureHook = hooks[0];
        textures[lc].assigned = true;
    }
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

bool ShaderManager::SetUniformMat4(std::string key, float* value_ptr)
{
    const auto location = glGetUniformLocation(shaderProgram, key.c_str());
    if (location == -1) return false;

    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr);
}

void ShaderManager::bindTextures()
{
    for (int lc=0; lc<16; lc++)
    {
        SetUniformInt("texture" + std::to_string(lc), lc);
        glActiveTexture(GL_TEXTURE0 + lc);
        glBindTexture(GL_TEXTURE_2D, textures[lc].textureHook);
    }
}

void ShaderManager::createTextures()
{
    for (int lc=0; lc<16; lc++)
    {
        glGenTextures(1, &(textures[lc].textureHook));
        textures[lc].assigned = true;
    }
}

void ShaderManager::Destroy()
{
    glDeleteShader(shaderProgram);
    shaderProgram = NULL;

    for (int lc=0; lc<16; lc++)
    {
        glDeleteTextures(1, &(textures[0].textureHook));
    }
}
