#include <iostream>
#include <vector>

#include "geometry/Ray.hpp"
#include "geometry/Plane.hpp"
#include "tracing/Trace.hpp"
#include "tracing/RayEmitter.hpp"
#include "animation/PathAnimation.hpp"
#include "animation/Playback.hpp"
#include "rendering/ShaderProgram.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/ShaderSources.hpp"
#include "rendering/Camera.hpp"
#include "scene/Room.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

using std::vector;

int main()
{
    // a problem to note is that rays hitting corners might not be accurately calculated
    // look at this later

    const double roomWidth = 10.0;
    const double roomDepth = 8.0;
    const double roomHeight = 6.0;

    vector<Plane> room =
        createRoomPlanes(roomWidth, roomDepth, roomHeight);

    const Vec3 origin(
        roomWidth / 2.0,
        roomDepth / 2.0,
        roomHeight / 2.0);

    const int ray_count = 10;
    const int max_bounces = 8;
    const int seed = 42;

    vector<vector<Vec3>> paths = generate_paths(origin, room, ray_count, max_bounces, seed);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window =
        glfwCreateWindow(1000, 700, "Acoustic Raytracer", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // reserve memory for the longest path
    std::size_t maxPathPoints = 0;

    for (const auto &rayPath : paths)
    {
        if (rayPath.size() > maxPathPoints)
        {
            maxPathPoints = rayPath.size();
        }
    }

    // create space for the longest path: three floats per point.
    vector<float> vertices(maxPathPoints * 3, 0.0f);

    // room vertices begin after the reusable ray space.
    const GLint roomFirstVertex =
        static_cast<GLint>(maxPathPoints);

    vector<float> roomVertices =
        createRoomWireframe(roomWidth, roomDepth, roomHeight);

    vertices.insert(
        vertices.end(),
        roomVertices.begin(),
        roomVertices.end());

    // uploads vertices data to the GPU
    VertexBuffer buffer = createVertexBuffer(vertices);

    GLuint vao = buffer.vao;
    GLuint vbo = buffer.vbo;

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, ShaderSources::vertex);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, ShaderSources::fragment);

    // check that they compile successfully
    if (!vertexShader || !fragmentShader)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLuint program = linkProgram(vertexShader, fragmentShader);

    if (!program)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glfwDestroyWindow(window);
        glfwTerminate();

        return 1;
    }

    // if linked successfully, we no longer need the shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint colorLocation = glGetUniformLocation(program, "drawColor");

    GLint viewProjectionLocation =
        glGetUniformLocation(program, "viewProjection");

    // camera target is the middle of the room
    const glm::vec3 cameraTarget(
        static_cast<float>(roomWidth / 2.0),
        static_cast<float>(roomDepth / 2.0),
        static_cast<float>(roomHeight / 2.0));

    float yaw = glm::radians(40.0f);
    float pitch = glm::radians(28.0f);
    float cameraDistance = 24.0f;

    double previousTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.10f, 0.14f, 1.0f);

    const double animationSpeed = 5.0;
    Playback playback;

    bool spaceWasPressed = false;
    bool restartWasPressed = false;

    vector<float> visibleVertices;
    visibleVertices.reserve(maxPathPoints * 3);

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        if (width == 0 || height == 0)
        {
            glfwWaitEvents();
            continue;
        }

        double currentTime = glfwGetTime();

        float deltaTime = glm::clamp(
            static_cast<float>(currentTime - previousTime),
            0.0f,
            0.05f);

        previousTime = currentTime;

        bool spacePressed =
            glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

        bool restartPressed =
            glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

        if (spacePressed && !spaceWasPressed)
        {
            playback.togglePause();
        }

        if (restartPressed && !restartWasPressed)
        {
            playback.restart();
        }
        else
        {
            playback.advance(deltaTime);
        }

        spaceWasPressed = spacePressed;
        restartWasPressed = restartPressed;

        const float rotationSpeed = glm::radians(60.0f);
        const float zoomSpeed = 10.0f;

        // horizontal rotation.
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            yaw -= rotationSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            yaw += rotationSpeed * deltaTime;

        // vertical rotation.
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            pitch += rotationSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            pitch -= rotationSpeed * deltaTime;

        // distance from the room's center.
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            cameraDistance -= zoomSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            cameraDistance += zoomSpeed * deltaTime;

        // keep the camera away from the vertical poles and outside the room.
        pitch = glm::clamp(
            pitch,
            glm::radians(-80.0f),
            glm::radians(80.0f));

        cameraDistance = glm::clamp(cameraDistance, 10.0f, 50.0f);

        // orbit in the XY plane; pitch raises the camera along Z.
        glm::mat4 view = createOrbitView(
            cameraTarget,
            yaw,
            pitch,
            cameraDistance);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(width) / height,
            0.1f,
            100.0f);

        glm::mat4 viewProjection = projection * view;

        glUseProgram(program);
        glUniformMatrix4fv(
            viewProjectionLocation,
            1,
            GL_FALSE,
            glm::value_ptr(viewProjection));

        glBindVertexArray(vao);

        // draw the room once per frame.
        glUniform3f(colorLocation, 0.55f, 0.60f, 0.65f);
        glDrawArrays(GL_LINES, roomFirstVertex, 24);

        double distanceTravelled = playback.getTime() * animationSpeed;

        glUniform3f(colorLocation, 0.0f, 0.5f, 1.0f);

        for (const auto &rayPath : paths)
        {
            vector<Vec3> visiblePath =
                get_visible_path(rayPath, distanceTravelled);

            const GLsizei visibleVertexCount =
                uploadPath(buffer, visiblePath, visibleVertices);

            glDrawArrays(GL_LINE_STRIP, 0, visibleVertexCount);
        }

        // draw the starting marker as a visible overlay.
        glDisable(GL_DEPTH_TEST);
        glUniform3f(colorLocation, 0.20f, 1.0f, 0.40f);
        glPointSize(12.0f);
        glDrawArrays(GL_POINTS, 0, 1);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();
}
