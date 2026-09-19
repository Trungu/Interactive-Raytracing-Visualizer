#include "../math/Vec3.hpp"
#include <vector>

std::vector<Vec3> get_visible_path(
    const std::vector<Vec3> &fullPath,
    double distanceTravelled);