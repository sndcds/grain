#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Math/Vec2.hpp>

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <span>

namespace Grain {

class BezierValueCurvePoint;

class Bezier {
public:
    Bezier() noexcept = default;

    Bezier(
        double x0,
        double y0,
        double x1,
        double y1,
        double x2,
        double y2,
        double x3,
        double y3
    ) noexcept;

    Bezier(
        const Vec2d& p0,
        const Vec2d& p1,
        const Vec2d& p2,
        const Vec2d& p3
    ) noexcept;

    Bezier(
        const Vec2d& p0,
        const Vec2d& p1,
        const Vec2d& p2
    ) noexcept;

    explicit Bezier(const Vec2d* positions) noexcept;

    Bezier(
        const BezierValueCurvePoint& p0,
        const BezierValueCurvePoint& p1
    ) noexcept;

    ~Bezier() noexcept = default;

    Bezier(const Bezier&) noexcept = default;
    Bezier& operator=(const Bezier&) noexcept = default;
    Bezier(Bezier&&) noexcept = default;
    Bezier& operator=(Bezier&&) noexcept = default;

    [[nodiscard]] Vec2d startPos() const noexcept;
    [[nodiscard]] Vec2d controlPos1() const noexcept;
    [[nodiscard]] Vec2d controlPos2() const noexcept;
    [[nodiscard]] Vec2d endPos() const noexcept;

    [[nodiscard]] Vec2d posAtPointIndex(int32_t index) const noexcept;
    [[nodiscard]] Vec2d posOnCurve(double t) const noexcept;

    [[nodiscard]] Rectd bounds() const noexcept;

    [[nodiscard]] double approximatedCurveLength(
        int32_t resolution = 100
    ) const noexcept;

    [[nodiscard]] Vec2d tangent1() const noexcept;
    [[nodiscard]] Vec2d tangent2() const noexcept;

    void set(
        const Vec2d& p0,
        const Vec2d& p1,
        const Vec2d& p2,
        const Vec2d& p3
    ) noexcept;

    void setQuadratic(
        const Vec2d& p0,
        const Vec2d& p1,
        const Vec2d& p2
    ) noexcept;

    void set(
        double x0,
        double y0,
        double x1,
        double y1,
        double x2,
        double y2,
        double x3,
        double y3
    ) noexcept;

    void setPointAtIndex(
        int32_t index,
        const Vec2d& position
    ) noexcept;

    void setHorizontalSegment(
        const Vec2d& left,
        const Vec2d& right,
        const Vec2d& leftFactor,
        const Vec2d& rightFactor
    ) noexcept;

    void setWithTangents(
        const Vec2d& start,
        const Vec2d& end,
        const Vec2d& tangent1,
        const Vec2d& tangent2
    ) noexcept;

    [[nodiscard]] double hit(
        const Vec2d& position,
        double radius = 6.0
    ) const noexcept;

    [[nodiscard]] bool hitBounds(
        const Vec2d& position,
        double radius = 6.0
    ) const noexcept;

    [[nodiscard]] int32_t hitPoint(
        const Vec2d& position,
        double radius = 6.0
    ) const noexcept;

    [[nodiscard]] bool split(
        double t,
        Bezier& left,
        Bezier& right
    ) const noexcept;

    [[nodiscard]] bool truncate(
        double start,
        double end,
        Bezier& result
    ) const noexcept;

    void buildVec2LUT(
        Vec2d* lut,
        int32_t resolution
    ) const noexcept;

    void translate(double x, double y) noexcept;
    void translate(const Vec2d& offset) noexcept;

    void translateX(double x) noexcept;
    void translateY(double y) noexcept;

    void scale(double x, double y) noexcept;
    void scale(const Vec2d& scale) noexcept;

    void scaleX(double x) noexcept;
    void scaleY(double y) noexcept;

    void transformByRect(const Rectd& rect) noexcept;

    void approximateQuadraticBezierControlPos(
        Vec2d& control
    ) const noexcept;

    [[nodiscard]] static int32_t arcToBezierPosArray(
        const Vec2d& start,
        const Vec2d& radii,
        double rotation,
        bool largeArcFlag,
        bool sweepFlag,
        const Vec2d& end,
        int32_t maxSegmentCount,
        Vec2d* output
    ) noexcept;

    [[nodiscard]]
    static std::optional<Bezier> fitCubicBezierToPoints(
        std::span<const Vec2d> points
    );

    friend std::ostream& operator<<(
        std::ostream& os,
        const Bezier& bezier
    );

private:
    [[nodiscard]] static double evaluate(
        double t,
        double p0,
        double p1,
        double p2,
        double p3
    ) noexcept;

    Vec2d pos_[4]{};
};

} // namespace Grain