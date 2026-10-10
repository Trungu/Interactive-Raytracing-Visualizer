#include <iostream>
#include <vector>
#include <limits>

#include "geometry/Ray.hpp"
#include "geometry/Receiver.hpp"
#include "geometry/Wall.hpp"
#include "tracing/Trace.hpp"
#include "tracing/RayEmitter.hpp"
#include "animation/PathAnimation.hpp"
#include "animation/Playback.hpp"
#include "rendering/ShaderProgram.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/ShaderSources.hpp"
#include "rendering/Camera.hpp"
#include "scene/Room.hpp"
#include "scene/Scene.hpp"

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

    // vector<Wall> room =
    //     createBoxRoom(roomWidth, roomDepth, roomHeight);

    Scene scene = createHouseScene();

    vector<Wall> room = scene.walls;
    Vec3 origin = scene.sourcePosition;

    // const Vec3 origin(
    //     roomWidth / 2.0,
    //     roomDepth / 2.0,
    //     roomHeight / 2.0);

    const int ray_count = 100;
    const int max_bounces = 8;
    // const int seed = 42;

    vector<vector<Vec3>> paths =
        generate_horizontal_paths(origin, room, ray_count, max_bounces);

    // Receiver receiver{Vec3(8.0, 6.0, 2.0), 1.5};

    Receiver receiver = scene.receiver;

    std::vector<double> highlightDistances;
    highlightDistances.reserve(paths.size());

    std::vector<double> arrivalDistances;
    arrivalDistances.reserve(paths.size());

    int pathsReachingReceiver = 0;

    auto updateReceiverHits = [&]()
    {
        highlightDistances.clear();
        arrivalDistances.clear();
        pathsReachingReceiver = 0;

        for (const auto &path : paths)
        {
            double distanceTravelled = 0;
            double highlightDistance = std::numeric_limits<double>::infinity();
            double arrivalDistance = std::numeric_limits<double>::infinity();

            for (std::size_t i = 1; i < path.size(); ++i)
            {
                const Vec3 &start = path[i - 1];
                const Vec3 &end = path[i];

                auto entry = receiver.entryDistance(start, end);

                if (entry)
                {
                    ++pathsReachingReceiver;

                    highlightDistance = distanceTravelled;
                    arrivalDistance = distanceTravelled + (*entry);

                    break;
                }

                distanceTravelled += (end - start).length();
            }

            highlightDistances.push_back(highlightDistance);
            arrivalDistances.push_back(arrivalDistance);
        }
    };

    updateReceiverHits();

    std::cout
        << "Paths reaching receiver: " << pathsReachingReceiver << " / " << paths.size() << '\n';

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

    // create space for the longest path: four floats per point.
    vector<float> vertices(maxPathPoints * 4, 0.0f);

    // room vertices begin after the reusable ray space.
    GLint roomFirstVertex =
        static_cast<GLint>(maxPathPoints);

    vector<float> roomVertices = createRoomWireframe(room);

    GLsizei roomVertexCount =
        static_cast<GLsizei>(roomVertices.size() / 3);

    for (size_t i = 0; i < roomVertices.size(); i += 3)
    {
        vertices.push_back(roomVertices[i]);
        vertices.push_back(roomVertices[i + 1]);
        vertices.push_back(roomVertices[i + 2]);
        vertices.push_back(1.0f);
    }

    GLint receiverVertex =
        static_cast<GLint>(vertices.size() / 4);

    vertices.push_back(static_cast<float>(receiver.position.x()));
    vertices.push_back(static_cast<float>(receiver.position.y()));
    vertices.push_back(static_cast<float>(receiver.position.z()));
    vertices.push_back(1.0f);

    GLint sourceVertex =
        static_cast<GLint>(vertices.size() / 4);

    vertices.push_back(static_cast<float>(origin.x()));
    vertices.push_back(static_cast<float>(origin.y()));
    vertices.push_back(static_cast<float>(origin.z()));
    vertices.push_back(1.0f);

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

    // Frame the center of the room's overall bounds.
    glm::vec3 cameraTarget(
        static_cast<float>(scene.cameraTarget.x()),
        static_cast<float>(scene.cameraTarget.y()),
        static_cast<float>(scene.cameraTarget.z()));

    float yaw = scene.cameraYaw;
    float pitch = scene.cameraPitch;
    float cameraDistance = scene.cameraDistance;

    double previousTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.10f, 0.14f, 1.0f);

    const double animationSpeed = 7.0;
    const double trailLength = 4.0;
    Playback playback;

    bool spaceWasPressed = false;
    bool restartWasPressed = false;

    bool one_was_pressed = false;
    bool two_was_pressed = false;
    bool three_was_pressed = false;

    vector<float> visibleVertices;
    visibleVertices.reserve(maxPathPoints * 4);

    auto loadScene = [&](const Scene &newScene)
    {
        room = newScene.walls;
        origin = newScene.sourcePosition;
        receiver = newScene.receiver;

        cameraTarget = glm::vec3(
            static_cast<float>(newScene.cameraTarget.x()),
            static_cast<float>(newScene.cameraTarget.y()),
            static_cast<float>(newScene.cameraTarget.z()));
        yaw = newScene.cameraYaw;
        cameraDistance = newScene.cameraDistance;
        pitch = newScene.cameraPitch;

        paths = generate_horizontal_paths(
            origin,
            room,
            ray_count,
            max_bounces);

        updateReceiverHits();

        maxPathPoints = 0;
        for (const auto &rayPath : paths)
        {
            if (rayPath.size() > maxPathPoints)
            {
                maxPathPoints = rayPath.size();
            }
        }

        vertices.assign(maxPathPoints * 4, 0.0f);

        roomFirstVertex =
            static_cast<GLint>(maxPathPoints);

        roomVertices = createRoomWireframe(room);

        roomVertexCount =
            static_cast<GLsizei>(roomVertices.size() / 3);

        for (size_t i = 0; i < roomVertices.size(); i += 3)
        {
            vertices.push_back(roomVertices[i]);
            vertices.push_back(roomVertices[i + 1]);
            vertices.push_back(roomVertices[i + 2]);
            vertices.push_back(1.0f);
        }

        receiverVertex =
            static_cast<GLint>(vertices.size() / 4);

        vertices.push_back(static_cast<float>(receiver.position.x()));
        vertices.push_back(static_cast<float>(receiver.position.y()));
        vertices.push_back(static_cast<float>(receiver.position.z()));
        vertices.push_back(1.0f);

        sourceVertex =
            static_cast<GLint>(vertices.size() / 4);

        vertices.push_back(static_cast<float>(origin.x()));
        vertices.push_back(static_cast<float>(origin.y()));
        vertices.push_back(static_cast<float>(origin.z()));
        vertices.push_back(1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
            vertices.data(),
            GL_DYNAMIC_DRAW);

        visibleVertices.clear();
        visibleVertices.reserve(maxPathPoints * 4);

        playback.restart();
        previousTime = glfwGetTime();
    };

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

        if (spacePressed && !spaceWasPressed)
        {
            playback.togglePause();
        }

        spaceWasPressed = spacePressed;

        bool restartPressed =
            glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

        if (restartPressed && !restartWasPressed)
        {
            playback.restart();
        }
        else
        {
            playback.advance(deltaTime);
        }

        restartWasPressed = restartPressed;

        bool onePressed =
            glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;

        bool twoPressed =
            glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;

        bool threePressed =
            glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS;

        if (onePressed && !one_was_pressed)
            loadScene(createBoxScene());

        else if (twoPressed && !two_was_pressed)
            loadScene(createLShapedScene());

        else if (threePressed && !three_was_pressed)
            loadScene(createHouseScene());

        one_was_pressed = onePressed;
        two_was_pressed = twoPressed;
        three_was_pressed = threePressed;

        const float rotationSpeed = glm::radians(60.0f);
        const float zoomSpeed = 10.0f;
        const float moveSpeed = 2.0f;

        // camera rotation in the XY plane.
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            yaw -= rotationSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            yaw += rotationSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            pitch += rotationSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            pitch -= rotationSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            cameraDistance -= zoomSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            cameraDistance += zoomSpeed * deltaTime;

        Vec3 previousReceiverPosition = receiver.position;

        // receiver movement in the XY plane.
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            receiver.position -= Vec3(moveSpeed * deltaTime, 0.0, 0.0);

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            receiver.position += Vec3(moveSpeed * deltaTime, 0.0, 0.0);

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            receiver.position += Vec3(0.0, moveSpeed * deltaTime, 0.0);

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            receiver.position -= Vec3(0.0, moveSpeed * deltaTime, 0.0);

        bool receiverMoved = (receiver.position - previousReceiverPosition).length_squared() > 0.0;

        if (receiverMoved)
        {
            updateReceiverHits();
        }

        float receiverPosition[3] = {
            static_cast<float>(receiver.position.x()),
            static_cast<float>(receiver.position.y()),
            static_cast<float>(receiver.position.z())};

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            static_cast<GLintptr>(receiverVertex * 4 * sizeof(float)),
            sizeof(receiverPosition),
            receiverPosition);

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
        glDrawArrays(GL_LINES, roomFirstVertex, roomVertexCount);

        double distanceTravelled = playback.getTime() * animationSpeed;
        double tailDistance = std::max(0.0, distanceTravelled - trailLength);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        for (size_t i = 0; i < paths.size(); ++i)
        {
            bool approachingReceiver =
                distanceTravelled >= highlightDistances[i] &&
                distanceTravelled < arrivalDistances[i];

            if (approachingReceiver)
                glUniform3f(colorLocation, 1.0f, 0.5f, 0.0f);
            else
                glUniform3f(colorLocation, 0.0f, 0.5f, 1.0f);

            vector<Vec3> visiblePath =
                get_path_section(paths[i], tailDistance, distanceTravelled);

            const GLsizei visibleVertexCount =
                uploadPath(buffer, visiblePath, visibleVertices);

            glDrawArrays(GL_LINE_STRIP, 0, visibleVertexCount);
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // draw the starting marker as a visible overlay.
        glDisable(GL_DEPTH_TEST);

        glUniform3f(colorLocation, 0.20f, 1.0f, 0.40f);
        glPointSize(12.0f);
        glDrawArrays(GL_POINTS, sourceVertex, 1);

        glUniform3f(colorLocation, 1.0f, 0.2f, 0.8f);
        glPointSize(12.0f);
        glDrawArrays(GL_POINTS, receiverVertex, 1);

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
