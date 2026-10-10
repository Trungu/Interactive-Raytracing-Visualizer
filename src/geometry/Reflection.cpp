#include "Reflection.hpp"

using std::optional;

constexpr double tolerance = 0.000001;

Vec3 get_reflection_dir(const Ray &incoming, const Vec3 &surface_normal)
{
    // defined as R = D - 2 N (N dot D), this returns the direction of the reflected ray
    // clang-format off
    Vec3 R = (incoming.direction() - (surface_normal * 2)
                 * (dot(incoming.direction(), surface_normal)));
    
    return R;
}

Ray generate_reflection(const Ray &incoming_ray, const Intersection &intersection) 
{
    Vec3 reflected_dir = get_reflection_dir(incoming_ray, intersection.normal);

    // Interior walls can be hit from either side. Offset toward the outgoing ray.
    Vec3 offset_normal = intersection.normal;
    if (dot(reflected_dir, offset_normal) < 0.0)
        offset_normal = -offset_normal;

    Vec3 origin_new = intersection.position
                         + (offset_normal * tolerance);

    return Ray(origin_new, reflected_dir);
}

