#pragma once

#include <glm/glm.hpp>

glm::mat4 createOrbitView(
    const glm::vec3 &target,
    float yaw,
    float pitch,
    float cameraDistance);
