#pragma once
#include "../math/Vec3.hpp"

// defines a `Ray` object with an origin and direction
class Ray
{
private:
    Vec3 origin_;
    Vec3 direction_; // will be normalized

public:
    Ray(Vec3 origin, Vec3 direction);

    Vec3 origin() const;
    Vec3 direction() const;

    // returns the point corresponding to a given t value
    Vec3 get_point(double t) const;
};