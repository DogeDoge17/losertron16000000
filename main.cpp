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

#if defined(linux)
#include "appicon.h"
#elif defined(_WIN32)
#include<windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{	
		if ((key == GLFW_KEY_EQUAL) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      up_press(); 
		}

		if ((key == GLFW_KEY_MINUS) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      down_press();
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
#elif defined(_WIN32)
	
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101)); 

	HICON hIconBig = LoadIcon(hInstance, MAKEINTRESOURCE(101));
	HICON hIconSmall = (HICON)LoadImage(hInstance,
		MAKEINTRESOURCE(101),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	if (hIconBig || hIconSmall) {
		HWND hwnd = glfwGetWin32Window(window);
		if (hwnd) {
			if (hIconBig)
				SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
			if (hIconSmall)
				SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
		}
	}

	if (hIcon) {
		
		HWND hwnd = glfwGetWin32Window(window);
		if (hwnd) {
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		}

		ICONINFO iconInfo;
		if (GetIconInfo(hIcon, &iconInfo)) {
			BITMAP bmp;
			if (GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmp)) {
				int width = bmp.bmWidth;
				int height = bmp.bmHeight;

				
				HDC hdc = CreateCompatibleDC(NULL);
				if (hdc) {
		
					BITMAPINFO bmi;
					ZeroMemory(&bmi, sizeof(BITMAPINFO));
					bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bmi.bmiHeader.biWidth = width;
					bmi.bmiHeader.biHeight = -height; 
					bmi.bmiHeader.biPlanes = 1;
					bmi.bmiHeader.biBitCount = 32;
					bmi.bmiHeader.biCompression = BI_RGB;

					BYTE* bits = nullptr;
					HBITMAP hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);

					if (hbmp && bits) {
						
						HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc, hbmp);
			
						DrawIconEx(hdc, 0, 0, hIcon, width, height, 0, NULL, DI_NORMAL);						
						unsigned char* pixels = new unsigned char[width * height * 4];
					
						for (int y = 0; y < height; y++) {
							for (int x = 0; x < width; x++) {
								int pixelPos = (y * width + x) * 4;
								int dibPos = (y * width + x) * 4;

								pixels[pixelPos + 0] = bits[dibPos + 2]; // R <- B
								pixels[pixelPos + 1] = bits[dibPos + 1]; // G <- G
								pixels[pixelPos + 2] = bits[dibPos + 0]; // B <- R
								pixels[pixelPos + 3] = bits[dibPos + 3]; // A <- A
							}
						}

						GLFWimage img = { 0 };
						img.width = width;
						img.height = height;
						img.pixels = pixels;
						glfwSetWindowIcon(window, 1, &img);

						delete[] pixels;
						SelectObject(hdc, hOldBmp);
						DeleteObject(hbmp);
					}
					DeleteDC(hdc);
				}
			}

			if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
			if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);
		}
		DestroyIcon(hIcon);
	}
	else {
		std::cerr << "Failed to load icon resource\n";
	}
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
