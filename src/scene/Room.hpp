#pragma once
#include <vector>
#include "../geometry/Plane.hpp"

std::vector<Plane> createRoomPlanes(
    double width,
    double depth,
    double height);

std::vector<float> createRoomWireframe(
    double width,
    double depth,
    double height);