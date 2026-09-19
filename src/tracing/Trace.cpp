#include "Trace.hpp"
#include "../geometry/Reflection.hpp"

using std::vector, std::optional, std::nullopt;

optional<Intersection> find_closest_hit(const Ray &incoming, const vector<Plane> &planes)
{
    optional<Intersection> closest_hit = nullopt;

    for (const auto &plane : planes)
    {
        optional<Intersection> intersection = calculate_intersection(incoming, plane);

        if (!intersection)
            continue;

        // an intersection exists, but we don't have a closest_hit yet
        if (!closest_hit)
        {
            closest_hit = intersection;
        }
        else if (intersection->t < closest_hit->t)
        {
            closest_hit = intersection;
        }
    }

    return closest_hit;
}

vector<Vec3> get_path(const Ray &intiial_ray, const vector<Plane> &planes, int max_bounces)
{
    vector<Vec3> path = {intiial_ray.origin()};
    Ray current_ray = intiial_ray;

    for (int i = 0; i < max_bounces; i++)
    {
        optional<Intersection> closest_hit = find_closest_hit(current_ray, planes);

        if (!closest_hit)
            break;

        path.push_back(closest_hit->position);

        Ray reflected_ray = generate_reflection(current_ray, *closest_hit);
        current_ray = reflected_ray;
    }

    return path;
}
