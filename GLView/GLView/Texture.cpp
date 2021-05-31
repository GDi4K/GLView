#include "Texture.h"
#include "stb_image.h"

bool Texture::Load()
{
    data = stbi_load(filePath.c_str(), &width, &height, &noChannels, 0);
    if (!data)
    {
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, textureHook);
    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB, width, height,
            0, GL_RGB, GL_UNSIGNED_BYTE, data
        );
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    loaded = true;

    return true;
}

void Texture::FreeData()
{
    stbi_image_free(data);
}
