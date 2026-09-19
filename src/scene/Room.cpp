#include "Room.hpp"
#include <stdexcept>

using std::vector, std::invalid_argument;

vector<Plane> createRoomPlanes(
    double width,
    double depth,
    double height)
{
    if (!(width > 0 && depth > 0 && height > 0))
    {
        throw invalid_argument("Room dimensions must be positive");
    }

    Plane left_wall = Plane({0, 0, 0}, {1, 0, 0});
    Plane right_wall = Plane({width, 0, 0}, {-1, 0, 0});

    Plane floor = Plane({0, 0, 0}, {0, 0, 1});
    Plane ceiling = Plane({0, 0, height}, {0, 0, -1});

    Plane back_wall = Plane({0, depth, 0}, {0, -1, 0});
    Plane front_wall = Plane({0, 0, 0}, {0, 1, 0});

    return {
        left_wall,
        right_wall,
        floor,
        ceiling,
        back_wall,
        front_wall};
}

vector<float> createRoomWireframe(
    double width,
    double depth,
    double height)
{
    if (!(width > 0 && depth > 0 && height > 0))
    {
        throw invalid_argument("Room dimensions must be positive");
    }

    const Vec3 corners[] = {
        {0, depth, 0},
        {width, depth, 0},
        {width, 0, 0},
        {0, 0, 0},
        {0, depth, height},
        {width, depth, height},
        {width, 0, height},
        {0, 0, height}};

    // Each pair names the endpoints of one room edge.
    const int edges[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Floor perimeter
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4}, // Ceiling perimeter
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7} // Vertical edges
    };

    vector<float> vertices;
    vertices.reserve(72);

    for (const auto &edge : edges)
    {
        for (int index : edge)
        {
            const Vec3 &point = corners[index];

            vertices.push_back(static_cast<float>(point.x()));
            vertices.push_back(static_cast<float>(point.y()));
            vertices.push_back(static_cast<float>(point.z()));
        }
    }

    return vertices;
}
