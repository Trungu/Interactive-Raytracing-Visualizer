#pragma once
#include "../math/Vec3.hpp"

// defines a `Plane` object with a point and normal vector
class Plane
{
private:
    Vec3 reference_point_;
    Vec3 normal_vector_;

public:
    Plane(Vec3 ref, Vec3 normal);

    Vec3 ref() const;
    Vec3 normal() const;
};