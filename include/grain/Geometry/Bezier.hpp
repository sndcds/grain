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
    std::array<Vec2d, 4> pos_{};

    static constexpr double kDefaultHitRadius = 6.0;
    static constexpr int32_t kDefaultCurveLengthResolution = 100;

    Bezier() = default;

    Bezier(const Vec2d& p0, const Vec2d& p1, const Vec2d& p2, const Vec2d& p3);
    Bezier(const Vec2d& p0, const Vec2d& p1, const Vec2d& p2);
    explicit Bezier(const Vec2d* positions);
    Bezier(const BezierValueCurvePoint& p0, const BezierValueCurvePoint& p1);

    ~Bezier() = default;

    friend auto operator << (std::ostream& os, const Bezier& bezier) -> std::ostream&;

    Bezier(const Bezier&) = default;

    auto operator=(const Bezier&) -> Bezier& = default;

    Bezier(Bezier&&) = default;

    auto operator=(Bezier&&) -> Bezier& = default;

    [[nodiscard]] auto startPos() const -> Vec2d;
    [[nodiscard]] auto controlPos1() const -> Vec2d;
    [[nodiscard]] auto controlPos2() const -> Vec2d;
    [[nodiscard]] auto endPos() const -> Vec2d;

    [[nodiscard]] auto posAtPointIndex(size_t index) const -> Vec2d;
    [[nodiscard]] auto posOnCurve(double t) const -> Vec2d;
    [[nodiscard]] auto bounds() const -> Rectd;

    [[nodiscard]] auto approximatedCurveLength(
        int32_t resolution = kDefaultCurveLengthResolution
    ) const -> double;

    [[nodiscard]] auto tangent1() const -> Vec2d;
    [[nodiscard]] auto tangent2() const -> Vec2d;

    void set(
        const Vec2d& p0,
        const Vec2d& p1,
        const Vec2d& p2,
        const Vec2d& p3
    );

    void setQuadratic(
        const Vec2d& p0,
        const Vec2d& p1,
        const Vec2d& p2
    );

    void set(
        double x0, double y0,
        double x1, double y1,
        double x2, double y2,
        double x3, double y3
    );

    void setPointAtIndex(size_t index, const Vec2d& position);

    void setHorizontalSegment(
        const Vec2d& left,
        const Vec2d& right,
        const Vec2d& leftFactor,
        const Vec2d& rightFactor
    );

    void setWithTangents(
        const Vec2d& start,
        const Vec2d& end,
        const Vec2d& tangent1,
        const Vec2d& tangent2
    );

    [[nodiscard]] auto hit(
        const Vec2d& position,
        double radius = kDefaultHitRadius
    ) const -> double;

    [[nodiscard]] auto hitBounds(
        const Vec2d& position,
        double radius = kDefaultHitRadius
    ) const -> bool;

    [[nodiscard]] auto hitPoint(
        const Vec2d& position,
        double radius = kDefaultHitRadius
    ) const -> int32_t;

    auto split(
        double t,
        Bezier& left,
        Bezier& right
    ) const -> bool;

    auto truncate(
        double start,
        double end,
        Bezier& result
    ) const -> bool;

    void buildVec2LUT(
        Vec2d* lut,
        int32_t resolution
    ) const;

    void translate(double x, double y);
    void translate(const Vec2d& offset);
    void translateX(double x);
    void translateY(double y);

    void scale(double x, double y);
    void scale(const Vec2d& scale);
    void scaleX(double x);
    void scaleY(double y);

    void transformByRect(const Rectd& rect);

    void approximateQuadraticBezierControlPos(Vec2d& control) const;

    [[nodiscard]] static auto arcToBezierPosArray(
        const Vec2d& start,
        const Vec2d& radii,
        double rotation,
        bool largeArcFlag,
        bool sweepFlag,
        const Vec2d& end,
        int32_t maxSegmentCount,
        Vec2d* output
    ) -> int32_t;

    [[nodiscard]] static auto fitCubicBezierToPoints(
        std::span<const Vec2d> points
    ) -> std::optional<Bezier>;

private:
    [[nodiscard]] static auto evaluate(
        double t,
        double p0,
        double p1,
        double p2,
        double p3
    ) -> double;
};

} // namespace Grain