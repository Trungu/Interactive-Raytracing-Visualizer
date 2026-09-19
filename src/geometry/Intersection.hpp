#pragma once
#include "../math/Vec3.hpp"
#include <optional>
#include "Ray.hpp"
#include "Plane.hpp"

// a read-only objecting representing an Intersection.
struct Intersection
{
    double t;
    Vec3 position;
    Vec3 normal;
};

// returns an `Intersection` object given a ray and plane
// otherwise, an empty `std::optional` for a non-valid intersection
std::optional<Intersection> calculate_intersection(const Ray &ray, const Plane &plane);
