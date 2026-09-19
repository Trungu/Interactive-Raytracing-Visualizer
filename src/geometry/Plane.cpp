#include "Plane.hpp"
#include "stdexcept"

Plane::Plane(Vec3 ref, Vec3 normal)
{
    if (normal.length() == 0)
        throw std::invalid_argument("Normal vector cannot be of length zero");

    reference_point_ = ref;
    normal_vector_ = normal / normal.length();
}

Vec3 Plane::ref() const
{
    return reference_point_;
}

Vec3 Plane::normal() const
{
    return normal_vector_;
}