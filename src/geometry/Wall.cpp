#include "Wall.hpp"
#include <stdexcept>
#include <cmath>

using std::invalid_argument;

constexpr double tol = 1e-6;

Wall::Wall(Plane plane_ref, Vec3 max_corner, Vec3 min_corner)
    : _reference_plane(plane_ref), _max_corner(max_corner), _min_corner(min_corner)
{
    auto onPlane = [&plane_ref](Vec3 coords)
    {
        return std::abs(dot(plane_ref.normal(), coords - plane_ref.ref())) <= tol;
    };

    auto fixedAxis = [&max_corner, &min_corner]()
    {
        int fixed_count = 0;
        Axis fixed = Axis::None;

        if (std::abs(max_corner.x() - min_corner.x()) <= tol)
        {
            fixed = Axis::X;
            fixed_count++;
        }

        if (std::abs(max_corner.y() - min_corner.y()) <= tol)
        {
            fixed = Axis::Y;
            fixed_count++;
        }

        if (std::abs(max_corner.z() - min_corner.z()) <= tol)
        {
            fixed = Axis::Z;
            fixed_count++;
        }

        return fixed_count == 1 ? fixed : Axis::None;
    };

    if (!onPlane(max_corner) || !onPlane(min_corner))
        throw invalid_argument("Corner coordinates must be within the plane");

    if (min_corner.x() > max_corner.x() ||
        min_corner.y() > max_corner.y() ||
        min_corner.z() > max_corner.z())
        throw invalid_argument("Bounds are not ordered");

    Axis fixed = fixedAxis();

    if (fixed == Axis::None)
        throw invalid_argument("More than one axis or no axis is fixed");

    Vec3 axisDirection(
        fixed == Axis::X ? 1.0 : 0.0,
        fixed == Axis::Y ? 1.0 : 0.0,
        fixed == Axis::Z ? 1.0 : 0.0);

    if (cross(plane_ref.normal(), axisDirection).length() > tol)
        throw invalid_argument("Normal vector not aligned with fixed axis");
}

Vec3 Wall::minCorner() const
{
    return _min_corner;
}

Vec3 Wall::maxCorner() const
{
    return _max_corner;
}

Plane Wall::plane() const
{
    return _reference_plane;
}

bool Wall::containsPoint(const Vec3 &point) const
{

    if (std::abs(dot(_reference_plane.normal(), point - _reference_plane.ref())) > tol)
        return false;

    if (!(_min_corner.x() - tol <= point.x() && point.x() <= _max_corner.x() + tol) ||
        !(_min_corner.y() - tol <= point.y() && point.y() <= _max_corner.y() + tol) ||
        !(_min_corner.z() - tol <= point.z() && point.z() <= _max_corner.z() + tol))
        return false;

    return true;
}