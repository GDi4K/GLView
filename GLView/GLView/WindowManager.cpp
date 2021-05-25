#include "WindowManager.h"

WindowManager* WindowManager::current = nullptr;

WindowManager::~WindowManager()
{
   Destroy();
}

void WindowManager::Destroy()
{
   glfwTerminate();
}

void WindowManager::ResizeWindow(int w, int h)
{
   width = w;
   height = h;
   glViewport(0, 0, w, h);
}

Float2 WindowManager::GetMousePosition()
{
   double x, y;
   glfwGetCursorPos(window, &x, &y);
   // in GLFW origin is the top-left, but on OpenGL it's bottom left
   // So, we have to correct the y coordinates like this
   return { static_cast<float>(x), static_cast<float>(height - y) };
}

WindowManager* WindowManager::Get()
{
   if (!current)
   {
      WindowManager::current = new WindowManager();
   }

   return WindowManager::current;
}

void WindowManager::OnResizeWindow(GLFWwindow* window, int w, int h)
{
   WindowManager::Get()->ResizeWindow(w, h);
}

bool WindowManager::Init(int w, int h)
{
   width = w;
   height = w;
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

   glfwSetWindowSizeCallback(window, OnResizeWindow);

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
