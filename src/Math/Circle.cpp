#include <grain/Math/Circle.hpp>

#include <cmath>
#include <numbers>

namespace Grain::Circle {

double area(const double radius) noexcept{
    return std::numbers::pi * radius * radius;
}

double circumference(const double radius) noexcept{
    return 2.0 * std::numbers::pi * radius;
}

double radiusFromArea(const double area) noexcept{
    return std::sqrt(area / std::numbers::pi);
}

}