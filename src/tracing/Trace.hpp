#pragma once
#include <optional>
#include <vector>
#include "../geometry/Plane.hpp"
#include "../geometry/Intersection.hpp"
#include "../geometry/Wall.hpp"

std::optional<Intersection> find_closest_hit(const Ray &incoming, const std::vector<Wall> &walls);

std::vector<Vec3> get_path(const Ray &intiial_ray, const std::vector<Wall> &walls, int max_bounces);