#include <grain/Math/Sphere.hpp>

#include <numbers>

namespace Grain::Sphere {

auto area(const double radius) -> double {
    return 4.0 * std::numbers::pi * radius * radius;
}

auto volume(const double radius) -> double {
    return (4.0 / 3.0) * std::numbers::pi * radius * radius * radius;
}

}