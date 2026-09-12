#pragma once

#include <grain/Math/Vec2.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/RemapRect.hpp>
#include <grain/Geometry/Line.hpp>

#include <array>
#include <cstddef>

namespace Grain {

class Quadrilateral {
public:
    using PointArray = std::array<Vec2d, 4>;

    Quadrilateral() noexcept = default;

    explicit Quadrilateral(
        const Vec2d& p1,
        const Vec2d& p2,
        const Vec2d& p3,
        const Vec2d& p4
    ) noexcept;

    explicit Quadrilateral(
        const Vec2d& min,
        const Vec2d& max
    ) noexcept;

    explicit Quadrilateral(const Rectd& rect) noexcept;

    ~Quadrilateral() = default;

    Quadrilateral(const Quadrilateral&) = default;
    Quadrilateral& operator=(const Quadrilateral&) = default;
    Quadrilateral(Quadrilateral&&) noexcept = default;
    Quadrilateral& operator=(Quadrilateral&&) noexcept = default;

    // ---------------------------------------------------------------------
    // Points
    // ---------------------------------------------------------------------

    [[nodiscard]] const Vec2d& p1() const noexcept { return points_[0]; }
    [[nodiscard]] const Vec2d& p2() const noexcept { return points_[1]; }
    [[nodiscard]] const Vec2d& p3() const noexcept { return points_[2]; }
    [[nodiscard]] const Vec2d& p4() const noexcept { return points_[3]; }

    [[nodiscard]] const Vec2d& point(std::size_t index) const noexcept {
        return points_[index];
    }

    [[nodiscard]] Vec2d& point(std::size_t index) noexcept {
        return points_[index];
    }

    [[nodiscard]] const PointArray& points() const noexcept {
        return points_;
    }

    [[nodiscard]] PointArray& points() noexcept {
        return points_;
    }

    void set(
        const Vec2d& p1,
        const Vec2d& p2,
        const Vec2d& p3,
        const Vec2d& p4
    ) noexcept;

    void setPoint(std::size_t index, const Vec2d& point) noexcept;

    void setByMinMax(
        const Vec2d& min,
        const Vec2d& max
    ) noexcept;

    void setByRect(const Rectd& rect) noexcept;


    // ---------------------------------------------------------------------
    // State
    // ---------------------------------------------------------------------

    [[nodiscard]] bool isValid() const noexcept {
        return valid_;
    }

    [[nodiscard]] bool canProjectPerspective() const noexcept {
        return can_project_perspective_;
    }

    // ---------------------------------------------------------------------
    // Geometry
    // ---------------------------------------------------------------------

    [[nodiscard]] double area() const noexcept;

    [[nodiscard]] bool isSimple() const noexcept;

    [[nodiscard]] bool isConvex() const noexcept;

    [[nodiscard]] double flattestAngle() const noexcept;

    [[nodiscard]] bool contains(const Vec2d& point) const noexcept;

    [[nodiscard]] Vec2d centroid() const noexcept;

    // ---------------------------------------------------------------------
    // Parametric coordinates
    // ---------------------------------------------------------------------

    /**
     * Maps logical coordinates (u, v) in the unit square to the
     * corresponding physical position in the quadrilateral.
     */
    [[nodiscard]] Vec2d project(double u, double v) const noexcept;

    /**
     * Maps logical coordinates (u, v) to a physical position.
     */
    [[nodiscard]] Vec2d project(const Vec2d& uv) const noexcept;

    /**
     * Maps a physical position to logical coordinates (u, v).
     */
    [[nodiscard]] Vec2d map(const Vec2d& position) const noexcept;

    [[nodiscard]] Vec2d map(double x, double y) const noexcept;

    [[nodiscard]] bool project(
        const Vec2d& uv,
        Vec2d& result
    ) const noexcept;

    [[nodiscard]] bool map(
        double x,
        double y,
        Vec2d& result
    ) const noexcept;

    void projectPoints(
        Vec2d* points,
        std::size_t count
    ) const noexcept;

    // ---------------------------------------------------------------------
    // Lines
    // ---------------------------------------------------------------------

    [[nodiscard]] Lined horizontalLine(double v) const noexcept;

    [[nodiscard]] Lined verticalLine(double u) const noexcept;

    // ---------------------------------------------------------------------
    // Bézier helpers
    // ---------------------------------------------------------------------

    /**
     * Returns twelve points describing a Bézier approximation of a circle
     * mapped into the quadrilateral.
     */
    [[nodiscard]] std::array<Vec2d, 12> bezierCirclePoints() const noexcept;

    // ---------------------------------------------------------------------
    // Transformation
    // ---------------------------------------------------------------------

    void remap(const RemapRectd& rect) noexcept;

private:
    bool solvePerspective() noexcept;

    PointArray points_{
        Vec2d(-1.0, -1.0),
        Vec2d( 1.0, -1.0),
        Vec2d( 1.0,  1.0),
        Vec2d(-1.0,  1.0)
    };

    bool valid_ = false;
    bool can_project_perspective_ = false;

    // Homography coefficients.
    double a_ = 0.0;
    double b_ = 0.0;
    double d_ = 0.0;
    double e_ = 0.0;
    double g_ = 0.0;
    double h_ = 0.0;
};

} // namespace Grain