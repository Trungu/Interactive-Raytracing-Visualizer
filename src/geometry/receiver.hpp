#pragma once
#include "../math/Vec3.hpp"

struct Receiver
{
    Vec3 position;
    double radius;

    bool intersect(
        const Vec3 &start,
        const Vec3 &end) const;
};