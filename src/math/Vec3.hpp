#pragma once
#include <iostream>

class Vec3
{
private:
    double x_;
    double y_;
    double z_;

public:
    // constructors
    Vec3();
    Vec3(double x, double y, double z);

    // getter methods
    double x() const;
    double y() const;
    double z() const;

    double length() const;
    double length_squared() const;
    Vec3 norm() const;

    // vector operations
    Vec3 operator+(const Vec3 &other_vec) const;
    Vec3 operator-(const Vec3 &other_vec) const;

    void operator+=(const Vec3 &other_vec);
    void operator-=(const Vec3 &other_vec);
    void operator*=(double s);
    void operator/=(double s);

    // unary negation
    Vec3 operator-() const;

    // scalar operations
    Vec3 operator*(double s) const;
    Vec3 operator/(double s) const;
};

// other vector operations
double dot(const Vec3 &a, const Vec3 &b);
Vec3 cross(const Vec3 &a, const Vec3 &b);
std::ostream &operator<<(std::ostream &out, const Vec3 &v);
