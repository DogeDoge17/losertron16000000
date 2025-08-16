#include "bitmap.hpp"
#include <GL/gl.h>
#include <algorithm> // Add this include for std::max
#include "doki.hpp"

#if defined(_WIN32)
  #include <windows.h>
  #include <shlobj.h>      // SHGetKnownFolderPath
  #include <knownfolders.h>
  #include <memory>
#else
  #include <pwd.h>
  #include <sys/types.h>
  #include <unistd.h>
#endif

#include "json.hpp"

GLuint dokiFbo;

float quadVertices[] = {
	// positions    // texCoords
	-1.0f, -1.0f,   0.0f, 0.0f,
	 1.0f, -1.0f,   1.0f, 0.0f,
	 1.0f,  1.0f,   1.0f, 1.0f,
	-1.0f,  1.0f,   0.0f, 1.0f
};
unsigned int quadIndices[] = {
	0, 1, 2,
	2, 3, 0
};

GLuint VAO, VBO, EBO, program;
GLint drawPositionLoc, textureLoc;

void print_gl_errors()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << err << std::endl;
	}
}
std::filesystem::path user_home_directory() {
#if defined(_WIN32)
	PWSTR wpath = nullptr;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &wpath);
	if (SUCCEEDED(hr) && wpath) {
		std::filesystem::path p = std::filesystem::path(wpath);
		CoTaskMemFree(wpath);
		return p;
	}
	if (wpath) CoTaskMemFree(wpath);

	if (const wchar_t* up = _wgetenv(L"USERPROFILE"); up && *up) {
		return std::filesystem::path(up);
	}
	const wchar_t* hd = _wgetenv(L"HOMEDRIVE");
	const wchar_t* hp = _wgetenv(L"HOMEPATH");
	if (hd && *hd && hp && *hp) {
		return std::filesystem::path(std::wstring(hd) + std::wstring(hp));
	}
	throw std::runtime_error("Could not determine user home directory on Windows");
#else
	if (const char* home = std::getenv("HOME"); home && *home) {
		return std::filesystem::path(home);
	}

	if (struct passwd* pw = getpwuid(getuid()); pw && pw->pw_dir && *pw->pw_dir) {
		return std::filesystem::path(pw->pw_dir);
	}
	throw std::runtime_error("Could not determine user home directory on POSIX");
#endif
}


void save_doki() {
	constexpr int width = 960;
	constexpr int height = 960;
	uint8_t *pixels = new uint8_t[width * height * 4];
	glBindFramebuffer(GL_FRAMEBUFFER, dokiFbo);
	glReadPixels(0, 0, 960, 960, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	std::filesystem::path basePath = user_home_directory() / "Pictures/losertron";
	std::filesystem::path dokiPath;

	std::string girlS = std::string(girlsv[girl])+"-";
	int i = 0;
	for (; std::filesystem::exists(dokiPath = (basePath / (girlS + std::to_string(i) + ".png"))); i++);

	std::cout << "Saving doki to: " << dokiPath << std::endl;

	if (!std::filesystem::exists(basePath)) {
		std::filesystem::create_directories(basePath);
	}


	(void)stbi_write_png(dokiPath.string().c_str(), width, height, 4, pixels, width * 4);

	delete[] pixels;
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void placing_prepare() {
	glBindFramebuffer(GL_FRAMEBUFFER, dokiFbo);
	glViewport(0, 0, 960, 960);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(VAO);
	glUseProgram(program);
	print_gl_errors();
}

void placing_finish() {

	uint8_t *pixels = new uint8_t[960 * 960 * 4];
	glBindFramebuffer(GL_FRAMEBUFFER, dokiFbo);
	glReadPixels(0, 0, 960, 960, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	renderedDokiBounds = crop_image(pixels, 960, 960);
	delete[] pixels;

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void place_image(GLuint texture, ImVec2 position) {
    glUniform1i(textureLoc, 0);
	glUniform2i(drawPositionLoc, static_cast<int>(position.x), static_cast<int>(position.y));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    print_gl_errors();
}


GLuint create_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
    }
    return shader;
}

void init_doki_off_screen() {
	glGenFramebuffers(1, &dokiFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, dokiFbo);

	glGenTextures(1, &renderedDokiTexture);
	glBindTexture(GL_TEXTURE_2D, renderedDokiTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 960, 960, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedDokiTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// set up vertex array and buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// shader program
	const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

	const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 TexCoord;

        uniform sampler2D textureSampler;
		uniform ivec2 position;

        void main() {
			vec2 uvOffset = vec2(position) / vec2(textureSize(textureSampler, 0));
			vec2 uv = TexCoord + uvOffset;

			vec4 c = texture(textureSampler, uv);
			FragColor = vec4(c.rgb * c.a, c.a);

        }
    )";

	GLuint vertexShader = create_shader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = create_shader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cerr << "Shader program linking error: " << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(program);
	textureLoc = glGetUniformLocation(program, "textureSampler");
	drawPositionLoc = glGetUniformLocation(program, "position");
	if (drawPositionLoc != -1) glUniform2i(drawPositionLoc, 0, 0);
	glUseProgram(0);

	print_gl_errors();
}

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
				//minH = std::max(0, minH - extendBottom);
				minH = 0 > minH - extendBottom ? 0 : minH - extendBottom;
       
				//maxH = std::min(height - 1, maxH + extendTop);
				maxH = height - 1 < maxH + extendTop ? height - 1 : maxH + extendTop;
		}
		else if (cropHeight > cropWidth) {
				const int diff = cropHeight - cropWidth;
				const int extendLeft = diff / 2;
				const int extendRight = diff - extendLeft;
				minW = 0 > minW - extendLeft ? 0 : minW - extendLeft;
				maxW = width - 1 < maxW + extendRight ? width - 1: maxW + extendRight;
				// minW = std::max(0, minW - extendLeft);
				// maxW = std::min(width - 1, maxW + extendRight);
		}

	Bounds bounds(static_cast<float>(maxW) / static_cast<float>(width), static_cast<float>(maxH) / static_cast<float>(height),
			static_cast<float>(minW) / static_cast<float>(width), static_cast<float>(minH) / static_cast<float>(height));;

	bounds.x = minW;
	bounds.y = minH;
	bounds.width = cropWidth + 1;
	bounds.height = cropHeight + 1;

	return bounds;
}
