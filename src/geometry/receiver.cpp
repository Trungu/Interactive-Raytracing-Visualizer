#include "Receiver.hpp"
#include <algorithm>
#include <cmath>

std::optional<double> Receiver::entryDistance(const Vec3 &start, const Vec3 &end) const
{

    Vec3 toReceiver = position - start;

    if (toReceiver.length() <= radius)
        return 0.0;

    Vec3 segment = end - start;
    double segmentLength = segment.length();

    if (segmentLength == 0.0)
        return std::nullopt;

    Vec3 direction = segment / segmentLength;

    double proj = dot(toReceiver, direction);

    Vec3 perpendicular = toReceiver - direction * proj;
    double perpendicularSquared = perpendicular.length_squared();

    // if the closest point is outside the radius, the whole line misses.
    if (perpendicularSquared > radius * radius)
        return std::nullopt;

    double d = std::sqrt(radius * radius - perpendicularSquared);

    // distance to entry point
    double entry = proj - d;

    if (entry < 0.0 || entry > segmentLength)
        return std::nullopt;

    return entry;
}
