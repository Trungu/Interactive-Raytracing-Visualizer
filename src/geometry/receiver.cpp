#include "receiver.hpp"
#include <algorithm>

bool Receiver::intersect(const Vec3 &start, const Vec3 &end) const
{
    Vec3 segment = end - start;
    double lengthSquared = segment.length_squared();

    if (lengthSquared == 0)
        return (start - position).length() <= radius;

    Vec3 toReceiver = position - start;
    double t = dot(toReceiver, segment) / lengthSquared;

    t = std::clamp(t, 0.0, 1.0);

    Vec3 closest_point = start + (segment * t);

    return (closest_point - position).length() <= radius;
}
