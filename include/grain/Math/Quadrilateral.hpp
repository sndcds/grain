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

    Quadrilateral() = default;

    explicit Quadrilateral(
        const Vec2d& p1,
        const Vec2d& p2,
        const Vec2d& p3,
        const Vec2d& p4
    );

    explicit Quadrilateral(const Vec2d& min, const Vec2d& max);

    explicit Quadrilateral(const Rectd& rect);

    ~Quadrilateral() = default;

    Quadrilateral(const Quadrilateral&) = default;
    auto operator = (const Quadrilateral&) -> Quadrilateral& = default;
    Quadrilateral(Quadrilateral&&) = default;
    auto operator = (Quadrilateral&&) -> Quadrilateral& = default;

    //--------------------------------------------------------------------------
    //  Points.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto p1() const -> const Vec2d& { return points_[0]; }
    [[nodiscard]] auto p2() const -> const Vec2d& { return points_[1]; }
    [[nodiscard]] auto p3() const -> const Vec2d& { return points_[2]; }
    [[nodiscard]] auto p4() const -> const Vec2d& { return points_[3]; }

    [[nodiscard]] auto point(std::size_t index) const -> const Vec2d& {
        return points_[index];
    }

    [[nodiscard]] auto point(std::size_t index) -> Vec2d& {
        return points_[index];
    }

    [[nodiscard]] auto points() const -> const PointArray& {
        return points_;
    }

    [[nodiscard]] auto points() -> PointArray& {
        return points_;
    }

    void set(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4);
    void setPoint(std::size_t index, const Vec2d& point);
    void setByMinMax(const Vec2d& min, const Vec2d& max);
    void setByRect(const Rectd& rect);


    //--------------------------------------------------------------------------
    //  State.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto isValid() const -> bool {
        return valid_;
    }

    [[nodiscard]] auto canProjectPerspective() const -> bool {
        return can_project_perspective_;
    }

    //--------------------------------------------------------------------------
    //  Geometry.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto area() const -> double;
    [[nodiscard]] auto isSimple() const -> bool;
    [[nodiscard]] auto isConvex() const -> bool;
    [[nodiscard]] auto flattestAngle() const -> double;
    [[nodiscard]] auto contains(const Vec2d& point) const -> bool;
    [[nodiscard]] auto centroid() const -> Vec2d;


    //--------------------------------------------------------------------------
    //  Parametric coordinates.
    //--------------------------------------------------------------------------

    /**
     * Maps logical coordinates (u, v) in the unit square to the
     * corresponding physical position in the quadrilateral.
     */
    [[nodiscard]] auto project(double u, double v) const -> Vec2d;

    /**
     * Maps logical coordinates (u, v) to a physical position.
     */
    [[nodiscard]] auto project(const Vec2d& uv) const -> Vec2d;

    /**
     * Maps a physical position to logical coordinates (u, v).
     */
    [[nodiscard]] auto map(const Vec2d& position) const -> Vec2d;
    [[nodiscard]] auto map(double x, double y) const -> Vec2d;

    [[nodiscard]] auto project(const Vec2d& uv, Vec2d& result) const -> bool;

    [[nodiscard]] auto map(double x, double y, Vec2d& result) const -> bool;

    void projectPoints(Vec2d* points, std::size_t count ) const;

    //--------------------------------------------------------------------------
    //  Lines.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto horizontalLine(double v) const -> Lined;
    [[nodiscard]] auto verticalLine(double u) const -> Lined;


    //--------------------------------------------------------------------------
    //  Bézier helpers.
    //--------------------------------------------------------------------------

    /**
     * Returns twelve points describing a Bézier approximation of a circle
     * mapped into the quadrilateral.
     */
    [[nodiscard]] auto bezierCirclePoints() const -> std::array<Vec2d, 12>;


    //--------------------------------------------------------------------------
    //  Transformation.
    //--------------------------------------------------------------------------

    void remap(const RemapRectd& rect);

private:
    auto solvePerspective() -> bool;

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