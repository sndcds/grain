#include <grain/Math/Circle.hpp>

#include <cmath>
#include <numbers>

namespace Grain::Circle {

auto area(const double radius) -> double{
    return std::numbers::pi * radius * radius;
}

auto circumference(const double radius) -> double{
    return 2.0 * std::numbers::pi * radius;
}

auto radiusFromArea(const double area) -> double{
    return std::sqrt(area / std::numbers::pi);
}

}