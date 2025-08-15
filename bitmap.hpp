#pragma once
#include <filesystem>
#include <glad/glad.h>
#include <GL/gl.h>
#include <iostream>
#include <stb_image.h>
#include "imgui.h"

struct Bounds {
    float u0, v0, u1, v1;
};

struct Texture {
    GLuint id = 0;
    GLsizei width = 0, height = 0;
    GLenum type = 0;
};

inline GLuint renderedDokiTexture;
inline Bounds renderedDokiBounds = {0.0f, 0.0f, 1.0f, 1.0f};

void placing_prepare();
void placing_finish();
void place_image(GLuint texture, ImVec2 position = ImVec2(0, 0));
void init_doki_off_screen();
GLuint texture_send(const uint8_t* pixels, const int& width, const int& height);
bool load_texture(const std::filesystem::path& path,  uint8_t*& pixels, int& width, int& height);
Bounds crop_image(const uint8_t* pixels, const int& width, const int& height);
