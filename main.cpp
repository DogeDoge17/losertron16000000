#include <glad/glad.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "doki.hpp"
#include "ui.hpp"
#include "appicon.h"

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{	
		if ((key == GLFW_KEY_EQUAL) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
				zoom += 0.15; 
		}

		if ((key == GLFW_KEY_MINUS) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
				zoom -= 0.15; 
		}
		if (zoom < 0.001) {
				zoom = 0.001;
		}
}

void load_app_icon(GLFWwindow* window) {
#if defined (linux)
	GLFWimage img = { 0 };
	int comp = 4;
	img.pixels = stbi_load_from_memory(appicon_png, static_cast<int>(appicon_png_len), &img.width, &img.height, &comp, comp);
	if (img.pixels) {
		glfwSetWindowIcon(window, 1, &img);
		stbi_image_free(img.pixels);
	} else {
		std::cerr << "Failed to load embedded icon\n";
	}
#elif defined(__WIN32__)

#endif
}

int main(int argc, char** argv) {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 450, "LOSERTRON16000000", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, KeyCallback);
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

	load_app_icon(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	const ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	init_doki_off_screen();
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