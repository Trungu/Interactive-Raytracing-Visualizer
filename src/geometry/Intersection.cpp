#include "Intersection.hpp"
#include <cmath>

using std::abs, std::optional, std::nullopt, std::isfinite;

constexpr double tolerance = 0.000001;

optional<Intersection> calculate_intersection(const Ray &ray, const Plane &plane)
{
    double n_d = dot(plane.normal(), ray.direction());

    // an intersection is invalid if the ray is parallel to the plane
    // for this we check if n_d is within a certain tolerance (i.e some value close to 0)
    if (abs(n_d) < tolerance || !isfinite(n_d))
        return nullopt;

    double t = (dot(plane.normal(), plane.ref() - ray.origin())) / n_d;

    // an intersection is invalid if t is not strictly greater than tolerance
    if (t <= tolerance || !isfinite(t))
        return nullopt;

    Vec3 pos = ray.origin() + (ray.direction() * t);

    return Intersection{t, pos, plane.normal()};
}
