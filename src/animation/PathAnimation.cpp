#include "PathAnimation.hpp"

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