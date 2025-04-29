#include "bitmap.hpp"




GLuint texture_send(const uint8_t* pixels, const int& width, const int& height) {
		GLuint texture = 0;
		glGenTextures(1, &texture);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
				std::cerr << "glGenTextures failed with error: 0x"
						<< std::hex << error << std::dec << std::endl;
				return 0;
		}
		if (texture == 0) {
				std::cerr << "glGenTextures returned texture id 0. OpenGL context may not be active." << std::endl;
				return 0;
		}

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		error = glGetError();
		if (error != GL_NO_ERROR) {
				std::cerr << "glTexImage2D failed with error: 0x"
						<< std::hex << error << std::dec << std::endl;
		}

		return texture;
}


bool load_texture(const std::filesystem::path& path, uint8_t*& pixels, int& width, int& height) {
		int channels;
		pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
		return pixels;
}

Bounds crop_image(const uint8_t* pixels, const int& width, const int& height) {
		int minW = width, minH = height, maxW = 0, maxH = 0;

		for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
						if (pixels[(y * width + x) * 4 + 3] <= 0)
								continue;

						if (minW > x) minW = x;
						if (minH > y) minH = y;
						if (maxW < x) maxW = x;
						if (maxH < y) maxH = y;
				}
		}

		const int cropHeight = maxH - minH;
		const int cropWidth = maxW - minW;

		if (cropWidth > cropHeight) {
				const int diff = cropWidth - cropHeight;
				const int extendTop = diff / 2;
				const int extendBottom = diff - extendTop;
				minH = std::max(0, minH - extendBottom);
				maxH = std::min(height - 1, maxH + extendTop);
		}
		else if (cropHeight > cropWidth) {
				const int diff = cropHeight - cropWidth;
				const int extendLeft = diff / 2;
				const int extendRight = diff - extendLeft;
				minW = std::max(0, minW - extendLeft);
				maxW = std::min(width - 1, maxW + extendRight);
		}

		return Bounds(static_cast<float>(maxW) / static_cast<float>(width), static_cast<float>(maxH) / static_cast<float>(height),
				static_cast<float>(minW) / static_cast<float>(width), static_cast<float>(minH) / static_cast<float>(height));

}
