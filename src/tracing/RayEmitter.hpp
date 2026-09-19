#pragma once

#include <vector>
#include "math/Vec3.hpp"
#include "geometry/Plane.hpp"

// Generates `ray_count` random ray paths from source through room,
// each with up to `max_bounces` reflections. `Seed` makes results repeatable.
std::vector<std::vector<Vec3>> generate_paths(
    const Vec3 &source,
    const std::vector<Plane> &room,
    int ray_count,
    int max_bounces,
    unsigned int seed);