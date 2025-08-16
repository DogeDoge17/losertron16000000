cmake_minimum_required(VERSION 3.15)
include(FetchContent)

project(losertron)

set(CMAKE_CXX_STANDARD 20)

if (LINUX)
# generate icon header for linux
find_program(XXD_EXECUTABLE xxd REQUIRED)
set(ICON_PNG ${CMAKE_SOURCE_DIR}/appicon.png)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(ICON_HEADER ${GENERATED_DIR}/appicon.h)

add_custom_command(
        OUTPUT ${ICON_HEADER}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_DIR}
        COMMAND ${XXD_EXECUTABLE} -n appicon_png -i ${ICON_PNG} ${ICON_HEADER}
        DEPENDS ${ICON_PNG}
        COMMENT "Embedding ${ICON_PNG} -> ${ICON_HEADER}"
        VERBATIM
)
endif()

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)

#set(GLFW_USE_WAYLAND ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Glad
add_library(glad vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.2 
)
FetchContent_MakeAvailable(imgui)

file(GLOB IMGUI_SRC
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
    vendor
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
        vendor/stb_image_write.h
)

add_executable(losertron ${SOURCE_FILES} ${ICON_HEADER})
target_link_libraries(losertron PUBLIC imgui glfw glad)
target_include_directories(losertron PUBLIC
        vendor
        vendor/glad/include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${glfw_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/glad/include
        ${GENERATED_DIR}
)

# move sprites to output
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dokis)
set(DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${SOURCE_DIR} DESTINATION ${DEST_DIR})


if (APPLE)
    target_link_libraries(losertron PRIVATE "-framework OpenGL")
elseif(WIN32)
    target_link_libraries(losertron PRIVATE opengl32)
else() # Linux
    find_package(OpenGL REQUIRED)
    target_link_libraries(losertron PRIVATE ${OPENGL_LIBRARIES} dl)
endif()
