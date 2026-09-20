#include <grain/Math/Sphere.hpp>

#include <numbers>

namespace Grain::Sphere {

double area(const double radius) noexcept {
    return 4.0 * std::numbers::pi * radius * radius;
}

double volume(const double radius) noexcept {
    return (4.0 / 3.0) * std::numbers::pi * radius * radius * radius;
}

}