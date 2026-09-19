#include "geometry/Intersection.hpp"
#include "TestRunner.hpp"
#include <optional>
#include <cmath>

using std::optional;

constexpr auto EXPECT_MISS = std::nullopt;

// check if the value is within some tolerance
bool near(double actual, double expected)
{
    return std::isfinite(actual) && std::abs(actual - expected) < 1e-9;
}

bool check_hit(
    const Ray &ray,
    const Plane &plane,
    const optional<Intersection> &expected)
{
    auto hit = calculate_intersection(ray, plane);

    // expecting a miss, will return if expected is empty
    if (!expected)
        return !hit;

    // expecting a hit, got miss
    if (!hit)
        return false;

    // clang-format off
    return near(hit->t,expected->t) 
        && near(hit->position.x(), expected->position.x())
        && near(hit->position.y(), expected->position.y())
        && near(hit->position.z(), expected->position.z())
        && near(hit->normal.x(), expected->normal.x())
        && near(hit->normal.y(), expected->normal.y())
        && near(hit->normal.z(), expected->normal.z());
}

int main()
{
    TestRunner tests;
    Plane plane({0, 0, 5}, {0, 0, 1});

    // Straight-on hit: t = 5 and position = (0, 0, 5).
    tests.check_test(
        check_hit(
            Ray({0, 0, 0}, {0, 0, 1}),
            plane,
            Intersection{5.0, {0, 0, 5}, {0, 0, 1}}
        ),
        "Straight-on hit"
    );

    // Parallel Ray: expects a miss
    tests.check_test(
        check_hit(
            Ray({0, 0, 0}, {1, 0, 0}),
            plane,
            EXPECT_MISS
        ),
        "Parallel Ray"
    );

    // Ray pointing away: the plane is behind the ray, so t = -5.
    tests.check_test(
        check_hit(
            Ray({0, 0, 0}, {0, 0, -1}),
            plane,
            EXPECT_MISS
        ),
        "Ray pointing away"
    );

    // Ray starting on the plane: t = 0 is not a forward hit.
    tests.check_test(
        check_hit(
            Ray({1, 2, 5}, {0, 0, 1}),
            plane,
            EXPECT_MISS
        ),
        "Ray starting on the plane"
    );

    // Angled hit: (3, 0, 4) normalizes to (3/5, 0, 4/5).
    // t = 5 / (4/5) = 6.25 and position = (4.75, -2, 5).
    tests.check_test(
        check_hit(
            Ray({1, -2, 0}, {3, 0, 4}),
            plane,
            Intersection{6.25, {4.75, -2, 5}, {0, 0, 1}}
        ),
        "Angled hit"
    );

    // CTest sees 0 as success and 1 as failure.
    return tests.finish();
}
