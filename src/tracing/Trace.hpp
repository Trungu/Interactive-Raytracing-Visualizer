#pragma once
#include <optional>
#include <vector>
#include "../geometry/Plane.hpp"
#include "../geometry/Intersection.hpp"

std::optional<Intersection> find_closest_hit(const Ray &incoming, const std::vector<Plane> &planes);

std::vector<Vec3> get_path(const Ray &intiial_ray, const std::vector<Plane> &planes, int max_bounces);