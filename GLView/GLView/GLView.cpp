#include <iostream>
#include <filesystem>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "WindowManager.h"
#include "ShaderManager.h"
#include "glm/gtc/type_ptr.hpp"

namespace fs = std::experimental::filesystem;

int main(int argc, char* argv[])
{    
    WindowManager* windowManager = WindowManager::Get();
    if(!windowManager->Init(1000, 1000)) return -1;

    ShaderManager shaderManager;
    // if (argc == 2)
    // {
    //     if(!shaderManager.LoadFragmentShader(argv[1])) return -1;
    // } else if (argc == 3)
    // {
    //     if(!shaderManager.LoadFragmentShader(argv[1])) return -1;
    //     if(!shaderManager.LoadVertexShader(argv[2])) return -1;
    // }

    shaderManager.LoadVertexShader("D:\\Tmp\\vertex.glsl");
    shaderManager.LoadFragmentShader("D:\\Tmp\\fragment.glsl");

    float vertices[] = {
        // vertices         // texture-coords
        -1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // top-left
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f,// top-right
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,// bottom-right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f// bottom-left
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 3, 2
    };

    auto transform = glm::mat4(1.f);

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

    shaderManager.createTextures();
    
    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    windowManager->InitImGui();
    shaderManager.Recompile();

    while(!windowManager->ShouldClose())
    {
        windowManager->ResetWindow();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        if (shaderManager.IsCompilingFailed())
        {
            if (!windowManager->ShouldSelectShaders())
            {
                windowManager->ShowError("Shader Compiler Error", shaderManager.GetCompilingErrors().c_str()); 
            }
        } else
        {
            shaderManager.Use();
            // Assign uniforms
            const auto time = static_cast<float>(glfwGetTime());
            shaderManager.SetUniformFloat("u_time", time);

            const auto res = windowManager->GetResolution(); 
            shaderManager.SetUniformFloat2("u_resolution", static_cast<float>(res.x), static_cast<float>(res.y));

            const auto mousePos = windowManager->GetMousePosition();
            shaderManager.SetUniformFloat2("u_mouse", mousePos.x, mousePos.y);

            // // Usually we do like this.
            // // rotate -> scale -> translate
            // // But here we do it the reverse order. That's because OpenGL uses a column vector.
            // // more on that: https://stackoverflow.com/a/33958564
            // transform = glm::mat4(1.0f);
            // transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
            // transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 1.0f));
            // transform = glm::rotate(transform, time * 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
            
            shaderManager.SetUniformMat4("u_transform", glm::value_ptr(transform));

            shaderManager.bindTextures();
            glBindVertexArray(VBO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);   
        }
        
        if (windowManager->ShouldSelectShaders())
        {
            windowManager->SelectShader(shaderManager);
        } else if(windowManager->ShouldReload())
        {
            shaderManager.Recompile();
        }
        
        windowManager->UpdateWindow();

        
    }
    
    return 0;
}
