#pragma once
#include <filesystem>
#include <glad/glad.h>
#include <GL/gl.h>
#include <iostream>
#include <stb_image.h>

struct Bounds {
    float u0, v0, u1, v1;
};

struct Texture {
    GLuint id = 0;
    GLsizei width = 0, height = 0;
    GLenum type = 0;
};

inline GLuint marvin;

GLuint texture_send(const uint8_t* pixels, const int& width, const int& height);
bool load_texture(const std::filesystem::path& path,  uint8_t*& pixels, int& width, int& height);
Bounds crop_image(const uint8_t* pixels, const int& width, const int& height);