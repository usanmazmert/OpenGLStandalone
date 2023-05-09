#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_internal.h"

#include "Shader.h"

#define SENT 4

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

	glfwPollEvents();

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /*Adjust the framerate by VSync*/
    glfwSwapInterval(1);

    /*Initialize glew*/
    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        float positions[] = {
            100.0f, 100.0f, 0.0f, 0.0f,
            200.0f, 100.0f, 1.0f, 0.0f,
            200.0f, 200.0f, 1.0f, 1.0f,
            100.0f, 200.0f, 0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1 ,2,
            2, 3 ,0
        };

        /*Enable blending*/
    
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


        /*Create Vertex Atrib Object to simplfy binding process again and again*/
        VertexArray va;
        
        /*Create the vertex buffer*/
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push(GL_FLOAT, 2);
        layout.Push(GL_FLOAT, 2);
        va.AddBuffer(vb, layout);

        /*Setting index buffers*/
        IndexBuffer ib(indices, 6);
        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

        /*Creating vertex shader*/
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();

        /*Setting color uniform*/
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


        /*Set Texture*/
        Texture texture("assets/textures/luxury black orijinal.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        /*Unbind everything*/
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();
        Renderer renderer;

        float r = 0.0f;
        float increment = 0.05f;

        glfwSwapInterval(1); // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style

        // Start the Dear ImGui frame
        bool showDemoWindow = true;
        bool p_open = true;
        static auto first_time = true;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);


        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {


            /* Render here */
            renderer.Clear();

            /*Set model view*/

            //shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * model;

                shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va,ib,shader);
            }

            {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * model;

                shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va,ib,shader);
            }

            
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();


            /*Dockspace example*/

            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


			dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode; 
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;


            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", &p_open, window_flags);
            if (!opt_padding)

                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
				if (first_time) {
					first_time = false;
					ImGui::DockBuilderRemoveNode(dockspace_id);
					ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
					ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

					auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
					auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);

                    /*The order of the builded dock panels important for first time view*/

					ImGui::DockBuilderDockWindow("Left", dock_id_left);
					ImGui::DockBuilderDockWindow("Down", dock_id_down);

					ImGui::DockBuilderFinish(dockspace_id);
				}

            }
            ImGui::End();

            //ImGui::Begin("MyWindow");
            //ImGui::End();
            ImGui::Begin("Left");
            ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, 960.0f);
            ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, 960.0f);
            ImGui::End();

            ImGui::Begin("Down");
            ImGui::End();



            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}