#pragma once

namespace Grain::Circle {

[[nodiscard]] auto area(double radius) -> double;
[[nodiscard]] auto circumference(double radius) -> double;
[[nodiscard]] auto radiusFromArea(double area) -> double;

}