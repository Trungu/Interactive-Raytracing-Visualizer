#include "Ray.hpp"
#include "stdexcept"

using std::invalid_argument;

Ray::Ray(Vec3 origin, Vec3 direction)
{
    origin_ = origin;

    if (direction.length() == 0)
        throw invalid_argument("Direction vector cannot be of length zero");

    direction_ = direction / direction.length();
}

Vec3 Ray::origin() const
{
    return origin_;
}

Vec3 Ray::direction() const
{
    return direction_;
}

Vec3 Ray::get_point(double t) const
{
    return origin_ + (direction_ * t);
}
