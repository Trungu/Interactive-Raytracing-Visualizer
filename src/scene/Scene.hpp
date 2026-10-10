#pragma once
#include "../math/Vec3.hpp"
#include "../geometry/Receiver.hpp"
#include "../geometry/Wall.hpp"
#include <vector>

struct Scene
{
    std::vector<Wall> walls;
    Receiver receiver;

    Vec3 cameraTarget;
    Vec3 sourcePosition;

    float cameraDistance;
    float cameraYaw;
    float cameraPitch;
};

Scene createBoxScene();
Scene createLShapedScene();
Scene createHouseScene();
