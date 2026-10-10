#pragma once
#include "Plane.hpp"
#include "../math/Vec3.hpp"

enum class Axis
{
    X,
    Y,
    Z,
    None
};

class Wall
{
private:
    Plane _reference_plane;
    Vec3 _max_corner;
    Vec3 _min_corner;

public:
    Wall(Plane plane_ref, Vec3 max_corner, Vec3 min_corner);

    Vec3 minCorner() const;
    Vec3 maxCorner() const;

    Plane plane() const;
    bool containsPoint(const Vec3 &point) const;
};