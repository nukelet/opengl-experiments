cmake_minimum_required(VERSION 3.0)
project(learnopengl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Variables
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)

# External dependencies (GLFW, GLAD, etc)
add_subdirectory(external)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${EXTERNAL_DIR}/glfw/include/
    ${EXTERNAL_DIR}/glad/include/
)

set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    glad
)

add_subdirectory(src)