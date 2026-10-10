#pragma once
#include "../math/Vec3.hpp"
#include <optional>

struct Receiver
{
    Vec3 position;
    double radius;

    std::optional<double> entryDistance(
        const Vec3 &start,
        const Vec3 &end) const;
};