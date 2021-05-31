#pragma once
#include <string>
#include "glad/glad.h"

class Texture
{
private:
    int width = 0;
    int height = 0;
    int noChannels = 0;
    unsigned char* data = nullptr;

public:
    std::string filePath;
    GLuint textureHook;
    bool loaded = false;
    bool assigned = false;
    bool pathAssigned = false;
    
public:
    bool Load();
    void FreeData();
    unsigned char*  GetData()
    {
        return data;
    }

    int GetWidth()
    {
        return width;
    }

    int GetHeight()
    {
        return height;
    }
};
