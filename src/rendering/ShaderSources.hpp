#pragma once

namespace ShaderSources
{
    inline constexpr char *vertex = R"(
        #version 330 core

        layout(location = 0) in vec3 position;
        layout(location = 1) in float opacity;

        uniform mat4 viewProjection;

        out float fragmentOpacity;

        void main()
        {
            gl_Position = viewProjection * vec4(position, 1.0);
            fragmentOpacity = opacity;
        }
    )";

    inline constexpr char *fragment = R"(
        #version 330 core

        uniform vec3 drawColor;

        in float fragmentOpacity;
        out vec4 fragmentColor;

        void main()
        {
            fragmentColor = vec4(drawColor, fragmentOpacity);
        }
    )";
}