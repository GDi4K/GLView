#include "WindowManager.h"

WindowManager::~WindowManager()
{
   Destroy();
}

void WindowManager::Destroy()
{
   glfwTerminate();
}

bool WindowManager::Init(int width, int height)
{
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   window = glfwCreateWindow(width, height, "GLView", nullptr, nullptr);
   if (!window)
   {
      std::cerr << "Error when loading the GLFW Window" << std::endl;
      return false;
   }
   glfwMakeContextCurrent(window);

   if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
   {
      std::cerr << "Error when loading GLAD" << std::endl;
      return false;
   }
   return true; 
}

void WindowManager::ResetWindow()
{
   glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
   // Add background color
   glClear(GL_COLOR_BUFFER_BIT);
}

void WindowManager::UpdateWindow()
{
   glfwSwapBuffers(window);
   glfwPollEvents();

   // trigger to close window with the ESC key
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
   {
      glfwSetWindowShouldClose(window, 1);
   }
}

bool WindowManager::ShouldClose()
{
   return glfwWindowShouldClose(window) == 0? false : true;
}
