#include "WindowManager.h"

#include "cwalk.h"

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

bool WindowManager::ShouldReload()
{
   return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

Float2 WindowManager::GetMousePosition()
{
   double x, y;
   glfwGetCursorPos(window, &x, &y);
   // in GLFW origin is the top-left, but on OpenGL it's bottom left
   // So, we have to correct the y coordinates like this
   return { static_cast<float>(x), static_cast<float>(height - y) };
}

void WindowManager::InitImGui()
{
   // imgui
   // Setup Dear ImGui context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void)io;
   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

   // Setup Dear ImGui style
   ImGui::StyleColorsDark();
   //ImGui::StyleColorsClassic();

   // Setup Platform/Renderer backends
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init("#version 130");
}

void WindowManager::ShowError(std::string title, std::string message)
{
   imguiUpdated = true;
   
   const ImGuiViewport* viewport = ImGui::GetMainViewport();
   ImGui::SetNextWindowPos(viewport->Pos);
   ImGui::SetNextWindowSize(viewport->Size);
   
   ImGui::NewFrame();
   ImGui::Begin(title.c_str());
   ImGui::Text(message.c_str());
   ImGui::End();
   ImGui::Render();
}


bool WindowManager::ShouldSelectShaders()
{
   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
   {
      startSelectingShaders = true;
      return true;
   }

   return startSelectingShaders;
}

void WindowManager::SelectShader(ShaderManager &shaderManager)
{
   imguiUpdated = true;
   const ImGuiViewport* viewport = ImGui::GetMainViewport();
   ImGui::SetNextWindowPos(viewport->Pos);
   ImGui::SetNextWindowSize(viewport->Size);

   ImGui::NewFrame();
   if(ImGui::Begin("Select Shaders"))
   {
      if(ImGui::Button("Select Fragment Shader"))
      {
         auto currPath = shaderManager.GetFragmentShaderPath();
         size_t dirStopAt;
         cwk_path_get_dirname(currPath.c_str(), &dirStopAt);
         fileDialog.SetPwd(currPath.substr(0, dirStopAt));
         isVertexShaderSelected = false;
         fileDialog.Open();
      }

      if(ImGui::Button("Select Vertex Shader"))
      {
         auto currPath = shaderManager.GetVertexShaderPath();
         size_t dirStopAt;
         cwk_path_get_dirname(currPath.c_str(), &dirStopAt);
         fileDialog.SetPwd(currPath.substr(0, dirStopAt));
         fileDialog.Open();
         isVertexShaderSelected = true;
         fileDialog.Open();
      }

      if (ImGui::Button("Cancel"))
      {
         startSelectingShaders = false;
      }
   }
   ImGui::End();
   
   fileDialog.Display();

   if(fileDialog.HasSelected())
   {
      const auto selectedFile = fileDialog.GetSelected().string();
      if (isVertexShaderSelected)
      {
         shaderManager.LoadVertexShader(selectedFile);
      } else
      {
         shaderManager.LoadFragmentShader(selectedFile);
      }

      shaderManager.Recompile();
      startSelectingShaders = false;
      fileDialog.ClearSelected();
   }
   
   ImGui::Render();
   
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
   imguiUpdated = false;
   glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
   // Add background color
   glClear(GL_COLOR_BUFFER_BIT);
}

void WindowManager::UpdateWindow()
{
   if (imguiUpdated)
   {
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   }
   
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
