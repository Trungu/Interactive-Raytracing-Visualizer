#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 createOrbitView(
    const glm::vec3 &target,
    float yaw,
    float pitch,
    float cameraDistance)
{
    // camera displacement relative to the target
    glm::vec3 cameraOffset(
        cameraDistance * glm::cos(pitch) * glm::sin(yaw),
        -cameraDistance * glm::cos(pitch) * glm::cos(yaw),
        cameraDistance * glm::sin(pitch));

    glm::vec3 cameraPosition = target + cameraOffset;

    glm::mat4 view = glm::lookAt(
        cameraPosition,
        target,
        glm::vec3(0.0f, 0.0f, 1.0f));

    return view;
}
