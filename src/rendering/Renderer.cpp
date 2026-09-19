#include "Renderer.hpp"

VertexBuffer createVertexBuffer(const std::vector<float> &vertices)
{
    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // allocate buffer storage
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
        vertices.data(),
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    return VertexBuffer{vao, vbo};
}

GLsizei uploadPath(
    const VertexBuffer &buffer,
    const std::vector<Vec3> &path,
    std::vector<float> &vertex_data)
{

    vertex_data.clear();

    for (const Vec3 &point : path)
    {
        vertex_data.push_back(static_cast<float>(point.x()));
        vertex_data.push_back(static_cast<float>(point.y()));
        vertex_data.push_back(static_cast<float>(point.z()));
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(vertex_data.size() * sizeof(float)),
        vertex_data.data());

    return static_cast<GLsizei>(vertex_data.size() / 3);
};