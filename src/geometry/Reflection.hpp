#pragma once
#include "../math/Vec3.hpp"
#include "Ray.hpp"
#include "Plane.hpp"
#include <optional>
#include "Intersection.hpp"

Vec3 get_reflection_dir(const Ray &incoming, const Vec3 &surface_normal);

Ray generate_reflection(const Ray &incoming_ray, const Intersection &intersection);
