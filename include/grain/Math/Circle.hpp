#pragma once

namespace Grain::Circle {

[[nodiscard]]
double area(double radius) noexcept;

[[nodiscard]]
double circumference(double radius) noexcept;

[[nodiscard]]
double radiusFromArea(double area) noexcept;

}