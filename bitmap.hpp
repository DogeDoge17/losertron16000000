#pragma once
#include <filesystem>
#include <glad/glad.h>
#include <GL/gl.h>
#include <iostream>
#include <stb_image.h>
#include "imgui.h"
#include <stb_image_write.h>


struct Bounds {
    float u0, v0, u1, v1;
    unsigned int x, y, width, height;
    Bounds(float u0 = 0.0f, float v0 = 0.0f, float u1 = 1.0f, float v1 = 1.0f)
        : u0(u0), v0(v0), u1(u1), v1(v1) {}

};

struct Texture {
    GLuint id = 0;
    GLsizei width = 0, height = 0;
    GLenum type = 0;
};

inline GLuint renderedDokiTexture;
inline Bounds renderedDokiBounds = {0.0f, 0.0f, 1.0f, 1.0f};

/// Saves the rendered doki image to the user's Pictures directory
void save_doki();

/// Prepares the OpenGL context for doki placing, setting up the framebuffer and shaders
void placing_prepare();

/// Finalizes the doki placing process, reading the pixels from the framebuffer and cropping the image
void placing_finish();

/// Draws an image to the off-screen framebuffer
/// @param texture the OpenGL texture ID of the image to draw
/// @param position the position to draw the image at, in pixels
void place_image(GLuint texture, ImVec2 position = ImVec2(0, 0));

/// Sets up the opengl framebuffer and shaders for rendering doki images off-screen
void init_doki_off_screen();

/// Sends loaded images to the GPU as OpenGL textures
/// @param pixels pixel data in RGBA format
/// @param width the width of the texture
/// @param height the height of the texture
/// @return an OpenGL texture ID
GLuint texture_send(const uint8_t* pixels, const int& width, const int& height);

/// Loads a texture from a file path into memory
/// @param path the file path to the texture
/// @param pixels pointer to the pixel data (will be allocated by this function)
/// @param width reference to store the width of the texture
/// @param height reference to store the height of the texture
/// @return true if the texture was loaded successfully
bool load_texture(const std::filesystem::path& path,  uint8_t*& pixels, int& width, int& height);

/// Crops the image to the bounds of the non-transparent pixels
/// @param pixels the pixel data of the image in RGBA format
/// @param width the width of the image
/// @param height the height of the image
/// @return a Bounds object containing the uv coordinates and dimensions of the cropped image
Bounds crop_image(const uint8_t* pixels, const int& width, const int& height);
