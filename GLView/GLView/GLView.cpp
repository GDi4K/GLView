#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "WindowManager.h"
#include "ShaderManager.h"

int main(int argc, char* argv[])
{
    WindowManager windowManager;
    if(!windowManager.Init(800, 600)) return -1;

    ShaderManager shaderManager;
    if(!shaderManager.Load()) return -1;

    float vertices[] = {
        -0.5f, 0.5f, 0.0f, // top-left
        0.5f, 0.5f, 0.0f, // top-right
        0.5f, -0.5f, 0.0f, // bottom-right
        -0.5f, -0.5f, 0.0f // bottom-left
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 3, 2
    };

    GLuint VBO, EBO, VAO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    
    // bind buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    // assign data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while(!windowManager.ShouldClose())
    {
        windowManager.ResetWindow();
        shaderManager.Use();

        glBindVertexArray(VBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        
        windowManager.UpdateWindow();
    }
    return 0;
}
