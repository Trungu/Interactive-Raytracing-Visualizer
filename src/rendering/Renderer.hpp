#pragma once

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <vector>
#include "math/Vec3.hpp"

// read-only object containing data for `vao` and `vbo`
struct VertexBuffer
{
    GLuint vao = 0;
    GLuint vbo = 0;
};

VertexBuffer createVertexBuffer(const std::vector<float> &vertices);

GLsizei uploadPath(
    const VertexBuffer &buffer,
    const std::vector<Vec3> &path,
    std::vector<float> &vertex_data);