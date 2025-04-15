cmake_minimum_required(VERSION 3.15)
project(losertron)

set(CMAKE_CXX_STANDARD 20)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# Glad
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# ImGui
file(GLOB IMGUI_SRC
        external/imgui/*.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
        external/imgui
        external/imgui/backends
        external/glfw/include
        external/glad/include
)
target_link_libraries(imgui PUBLIC glfw glad)


set(SOURCE_FILES
        main.cpp
        ui.cpp
        ui.hpp
        doki.cpp
        doki.hpp
        bitmap.cpp
        bitmap.hpp
)

add_executable(losertron ${SOURCE_FILES})
target_include_directories(losertron PUBLIC
        external/imgui
        external
        external/imgui/backends
        external/glfw/include
        external/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(losertron PUBLIC imgui glfw glad)

# Platform-specific dependencies

if (APPLE)
    target_link_libraries(losertron PRIVATE "-framework OpenGL")
elseif(WIN32)
    target_link_libraries(losertron PRIVATE opengl32)
else() # Linux
    find_package(OpenGL REQUIRED)
    target_link_libraries(losertron PRIVATE ${OPENGL_LIBRARIES} dl)
endif()