#pragma once

namespace ShaderSources
{
    inline constexpr const char *vertex = R"(
        #version 330 core

        layout(location = 0) in vec3 position;

        uniform mat4 viewProjection;

        void main()
        {
            gl_Position = viewProjection * vec4(position, 1.0);
        }
    )";

    inline constexpr const char *fragment = R"(
        #version 330 core

        uniform vec3 drawColor;
        out vec4 fragmentColor;

        void main()
        {
            fragmentColor = vec4(drawColor, 1.0);
        }
    )";
}