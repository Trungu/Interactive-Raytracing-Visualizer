#include "RayEmitter.hpp"
#include "Trace.hpp"
#include "geometry/Ray.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>
#include <stdexcept>

using std::vector, std::numbers::pi;

vector<vector<Vec3>> generate_paths(
    const Vec3 &source,
    const vector<Plane> &room,
    int ray_count,
    int max_bounces,
    unsigned int seed)
{
    // check if the arguments are valid (i.e > 0)
    if (ray_count <= 0 || max_bounces <= 0)
    {
        throw std::invalid_argument("Ray count and maximum bounces must be a positive number");
    }

    vector<vector<Vec3>> paths;
    paths.reserve(ray_count);

    // initialize a random generator with a given seed
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> random(0.0, 1.0);

    for (int i = 0; i < ray_count; i++)
    {
        // produces a random z in the range of [-1.0, 1.0)
        double z = 2.0 * random(generator) - 1.0;
        double angle = (2.0 * pi) * random(generator);

        double radius = std::sqrt(std::max(0.0, 1.0 - z * z));

        Vec3 direction(
            radius * std::cos(angle),
            radius * std::sin(angle),
            z //
        );

        Ray ray(source, direction);
        paths.push_back(get_path(ray, room, max_bounces));
    }

    return paths;
}