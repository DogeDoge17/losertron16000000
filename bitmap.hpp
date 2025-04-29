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
    GLuint id;
    GLsizei width, height;
    GLenum type;
};

GLuint texture_send(const uint8_t* pixels, const int& width, const int& height);
bool load_texture(const std::filesystem::path& path,  uint8_t*& pixels, int& width, int& height);
Bounds crop_image(const uint8_t* pixels, const int& width, const int& height);