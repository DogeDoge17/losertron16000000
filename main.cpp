#include <glad/glad.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "doki.hpp"
#include "ui.hpp"

int main() {
		glfwInit();
		GLFWwindow* window = glfwCreateWindow(800, 450, "LOSERTRON16000000", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); 

		gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

		IMGUI_CHECKVERSION(); 
		ImGui::CreateContext();
		const ImGuiIO& io = ImGui::GetIO();
		const ImVec2 windowSize = io.DisplaySize;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		init_toutes_dokis();
		init_doki();
		while (!glfwWindowShouldClose(window)) {
				glfwPollEvents();

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
				ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
				
				draw_ui();

				ImGui::Render();
				int display_w, display_h;
				glfwGetFramebufferSize(window, &display_w, &display_h);
				glViewport(0, 0, display_w, display_h);
				glClearColor(0.1f, 0.1f, 0.1f, .0f);
				glClear(GL_COLOR_BUFFER_BIT);
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				glfwSwapBuffers(window);
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();

		return 0;
}