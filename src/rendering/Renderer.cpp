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

    // position
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        nullptr);

    glEnableVertexAttribArray(0);

    // opacity
    glVertexAttribPointer(
        1,
        1,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        reinterpret_cast<void *>(3 * sizeof(float)));

    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    return VertexBuffer{vao, vbo};
}

GLsizei uploadPath(
    const VertexBuffer &buffer,
    const std::vector<Vec3> &path,
    std::vector<float> &vertex_data)
{

    vertex_data.clear();

    double totalLength = 0.0;

    for (size_t i = 1; i < path.size(); ++i)
    {
        totalLength += (path[i] - path[i - 1]).length();
    }

    double distanceFromTail = 0.0;

    for (size_t i = 0; i < path.size(); ++i)
    {
        if (i > 0)
        {
            distanceFromTail += (path[i] - path[i - 1]).length();
        }

        float opacity = totalLength > 0.0
                            ? static_cast<float>(distanceFromTail / totalLength)
                            : 1.0f;

        const Vec3 &point = path[i];
        vertex_data.push_back(static_cast<float>(point.x()));
        vertex_data.push_back(static_cast<float>(point.y()));
        vertex_data.push_back(static_cast<float>(point.z()));
        vertex_data.push_back(opacity);
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(vertex_data.size() * sizeof(float)),
        vertex_data.data());

    return static_cast<GLsizei>(vertex_data.size() / 4);
};