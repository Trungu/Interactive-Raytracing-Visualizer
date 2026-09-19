#include "Vec3.hpp"
#include <cmath>

using std::pow, std::sqrt;

// default constructor
Vec3::Vec3() : x_(0), y_(0), z_(0) {};

// set constructor
Vec3::Vec3(double x, double y, double z) : x_(x), y_(y), z_(z) {};

double Vec3::x() const
{
    return x_;
}

double Vec3::y() const
{
    return y_;
}

double Vec3::z() const
{
    return z_;
}

// length
double Vec3::length() const
{
    return sqrt(length_squared());
}

double Vec3::length_squared() const
{
    return pow(x_, 2) + pow(y_, 2) + pow(z_, 2);
}

// normalization or unit vector
Vec3 Vec3::norm() const
{
    return (*this) / length();
}

// vector operations
void Vec3::operator+=(const Vec3 &other_vec)
{
    x_ = x_ + other_vec.x();
    y_ = y_ + other_vec.y();
    z_ = z_ + other_vec.z();
}

void Vec3::operator-=(const Vec3 &other_vec)
{
    x_ = x_ - other_vec.x();
    y_ = y_ - other_vec.y();
    z_ = z_ - other_vec.z();
}

void Vec3::operator*=(double s)
{
    x_ = x_ * s;
    y_ = y_ * s;
    z_ = z_ * s;
}

void Vec3::operator/=(double s)
{
    x_ = x_ / s;
    y_ = y_ / s;
    z_ = z_ / s;
}

Vec3 Vec3::operator+(const Vec3 &other_vec) const
{
    Vec3 new_vec = (*this);
    new_vec += other_vec;

    return new_vec;
}

Vec3 Vec3::operator-(const Vec3 &other_vec) const
{
    Vec3 new_vec = (*this);
    new_vec -= other_vec;

    return new_vec;
}

Vec3 Vec3::operator*(double s) const
{
    Vec3 new_vec = (*this);
    new_vec *= s;

    return new_vec;
}

Vec3 Vec3::operator/(double s) const
{
    Vec3 new_vec = (*this);
    new_vec /= s;

    return new_vec;
}

// unary negation
Vec3 Vec3::operator-() const
{
    return Vec3(-x_, -y_, -z_);
}

// dot product
double dot(const Vec3 &a, const Vec3 &b)
{
    return (a.x() * b.x()) + (a.y() * b.y()) + (a.z() * b.z());
}

Vec3 cross(const Vec3 &a, const Vec3 &b)
{
    // calculated via discriminant method
    double x = (a.y() * b.z()) - (a.z() * b.y());
    double y = (a.z() * b.x()) - (a.x() * b.z());
    double z = (a.x() * b.y()) - (a.y() * b.x());

    return Vec3(x, y, z);
}

// helpers
std::ostream &operator<<(std::ostream &os, const Vec3 &v)
{
    return os << "[" << v.x() << ", " << v.y() << ", " << v.z() << "]";
}
