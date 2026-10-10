#include "PathAnimation.hpp"
#include <algorithm>

using std::vector;

vector<Vec3> get_visible_path(
    const vector<Vec3> &fullPath,
    double distanceTravelled)
{
    if (fullPath.empty())
        return {};

    vector<Vec3> visible_path{fullPath.front()};

    if (distanceTravelled <= 0)
        return visible_path;

    double remainingDistance = distanceTravelled;

    for (size_t i = 1; i < fullPath.size(); i++)
    {
        Vec3 start = fullPath[i - 1];
        Vec3 end = fullPath[i];

        double segment_length = (end - start).length();

        if (segment_length == 0)
            continue;

        if (remainingDistance >= segment_length)
        {
            remainingDistance = remainingDistance - segment_length;
            visible_path.push_back(end);

            if (remainingDistance <= 0.0)
                break;
        }
        else
        {
            double fraction = remainingDistance / segment_length;
            Vec3 tip = start + (end - start) * fraction;

            visible_path.push_back(tip);
            break;
        }
    }

    return visible_path;
}

// note review this function:
vector<Vec3> get_path_section(
    const vector<Vec3> &fullPath,
    double startDistance,
    double endDistance)
{
    vector<Vec3> section;

    startDistance = std::max(0.0, startDistance);

    if (fullPath.size() < 2 || endDistance <= startDistance)
        return section;

    double distanceSoFar = 0.0;

    for (std::size_t i = 1; i < fullPath.size(); ++i)
    {
        const Vec3 &start = fullPath[i - 1];
        Vec3 segment = fullPath[i] - start;
        double segmentLength = segment.length();

        if (segmentLength == 0.0)
            continue;

        double segmentEndDistance = distanceSoFar + segmentLength;

        double overlapStart = std::max(startDistance, distanceSoFar);
        double overlapEnd = std::min(endDistance, segmentEndDistance);

        if (overlapStart < overlapEnd)
        {
            double startFraction =
                (overlapStart - distanceSoFar) / segmentLength;

            double endFraction =
                (overlapEnd - distanceSoFar) / segmentLength;

            if (section.empty())
                section.push_back(start + segment * startFraction);

            section.push_back(start + segment * endFraction);
        }

        distanceSoFar = segmentEndDistance;

        if (distanceSoFar >= endDistance)
            break;
    }

    return section;
}