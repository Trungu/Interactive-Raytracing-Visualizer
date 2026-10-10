#include "Room.hpp"
#include <stdexcept>
#include <cmath>

using std::vector, std::invalid_argument;

vector<Wall> createBoxRoom(
    double width,
    double depth,
    double height)
{
    if (!(width > 0 && depth > 0 && height > 0))
    {
        throw invalid_argument("Room dimensions must be positive");
    }

    Wall left = Wall(Plane({0, 0, 0}, {1, 0, 0}), {0, depth, height}, {0, 0, 0});
    Wall right = Wall(Plane({width, 0, 0}, {-1, 0, 0}), {width, depth, height}, {width, 0, 0});

    Wall floor = Wall(Plane({0, 0, 0}, {0, 0, 1}), {width, depth, 0}, {0, 0, 0});
    Wall ceiling = Wall(Plane({0, 0, height}, {0, 0, -1}), {width, depth, height}, {0, 0, height});

    Wall front = Wall(Plane({0, 0, 0}, {0, 1, 0}), {width, 0, height}, {0, 0, 0});
    Wall back = Wall(Plane({0, depth, 0}, {0, -1, 0}), {width, depth, height}, {0, depth, 0});

    return {
        left,
        right,
        floor,
        ceiling,
        front,
        back};
}

vector<Wall> createLShapedRoom(
    double width,
    double depth,
    double height,
    double armThickness)
{
    if (!(width > 0 && depth > 0 && height > 0 && armThickness > 0) ||
        !(armThickness < width && armThickness < depth))
    {
        throw invalid_argument("Room dimensions must be positive");
    }

    Wall front = Wall(Plane({0, 0, 0}, {0, 1, 0}), {width, 0, height}, {0, 0, 0});
    Wall right = Wall(Plane({width, 0, 0}, {-1, 0, 0}), {width, armThickness, height}, {width, 0, 0});

    Wall back = Wall(Plane({0, depth, 0}, {0, -1, 0}), {armThickness, depth, height}, {0, depth, 0});
    Wall left = Wall(Plane({0, 0, 0}, {1, 0, 0}), {0, depth, height}, {0, 0, 0});

    Wall innerHorizontal = Wall(Plane({0, armThickness, 0}, {0, -1, 0}), {width, armThickness, height}, {armThickness, armThickness, 0});
    Wall innerVertical = Wall(Plane({armThickness, 0, 0}, {-1, 0, 0}), {armThickness, depth, height}, {armThickness, armThickness, 0});

    // floor
    Wall bottomFloor = Wall(Plane({0, 0, 0}, {0, 0, 1}), {width, armThickness, 0}, {0, 0, 0});
    Wall upperLeftFloor = Wall(Plane({0, armThickness, 0}, {0, 0, 1}), {armThickness, depth, 0}, {0, armThickness, 0});

    // ceiling
    Wall bottomCeiling = Wall(Plane({0, 0, height}, {0, 0, -1}), {width, armThickness, height}, {0, 0, height});
    Wall upperLeftCeiling = Wall(Plane({0, armThickness, height}, {0, 0, -1}), {armThickness, depth, height}, {0, armThickness, height});

    return {
        front,
        right,
        back,
        left,
        innerHorizontal,
        innerVertical,
        bottomFloor,
        upperLeftFloor,
        bottomCeiling,
        upperLeftCeiling};
}

vector<float> createRoomWireframe(const vector<Wall> &walls)
{
    vector<float> vertices;
    vertices.reserve(walls.size() * 24);

    for (const Wall &wall : walls)
    {
        Vec3 min = wall.minCorner();
        Vec3 max = wall.maxCorner();

        constexpr double tolerance = 1e-6;

        Vec3 a, b, c, d;

        if (std::abs(max.x() - min.x()) <= tolerance)
        {
            // left and right
            a = min;
            b = Vec3(min.x(), max.y(), min.z());
            c = max;
            d = Vec3(min.x(), min.y(), max.z());
        }
        else if (std::abs(max.y() - min.y()) <= tolerance)
        {
            // front and back
            a = min;
            b = Vec3(max.x(), min.y(), min.z());
            c = max;
            d = Vec3(min.x(), min.y(), max.z());
        }
        else if (std::abs(max.z() - min.z()) <= tolerance)
        {
            // floor and ceiling
            a = min;
            b = Vec3(max.x(), min.y(), min.z());
            c = max;
            d = Vec3(min.x(), max.y(), min.z());
        }

        const Vec3 endpoints[] = {a, b, b, c, c, d, d, a};

        for (const Vec3 &point : endpoints)
        {
            vertices.push_back(static_cast<float>(point.x()));
            vertices.push_back(static_cast<float>(point.y()));
            vertices.push_back(static_cast<float>(point.z()));
        }
    }

    return vertices;
}
