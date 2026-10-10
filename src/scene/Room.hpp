#pragma once
#include <vector>
#include "../geometry/Plane.hpp"
#include "../geometry/Wall.hpp"

std::vector<Wall> createBoxRoom(
    double width,
    double depth,
    double height);

std::vector<Wall> createLShapedRoom(
    double width,
    double depth,
    double height,
    double armThickness);

std::vector<float> createRoomWireframe(const std::vector<Wall> &walls);