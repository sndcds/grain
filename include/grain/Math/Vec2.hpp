//
//  Vec2.hpp
//
//  Part of GrainLib
//  https://grain.one
//

#pragma once

#include <grain/String/String.hpp>
// #include <grain/String/CSVString.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <ostream>
#include <type_traits>

namespace Grain {

class File;

template <typename T>
struct Vec2 {
public:
    T x{};
    T y{};

    // Construction
    constexpr Vec2() = default;
    constexpr Vec2(T x, T y)
        : x(x), y(y) {}

    constexpr Vec2(const Vec2&) = default;
    constexpr Vec2(Vec2&&) = default;

    template <typename U>
        requires (!std::same_as<U, T>)
    explicit constexpr Vec2(const Vec2<U>& v)
        : x(static_cast<T>(v.x)),
          y(static_cast<T>(v.y)) {}

    explicit Vec2(const char* csv, char delimiter = ',') {
        setByCSV(csv, delimiter);
    }

    explicit Vec2(const String& csv, char delimiter = ',') {
        setByCSV(csv, delimiter);
    }

    ~Vec2() = default;

    auto operator = (const Vec2&) -> Vec2& = default;
    auto operator = (Vec2&&) -> Vec2& = default;

    template <typename U>
        requires std::is_convertible_v<U, T>
    auto operator = (const Vec2<U>& v) -> Vec2& {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        return *this;
    }


    constexpr auto operator [] (std::size_t index) -> T& {
        return index == 0 ? x : y;
    }

    constexpr auto operator [] (std::size_t index) const -> const T& {
        return index == 0 ? x : y;
    }

    // Comparison
    [[nodiscard]] constexpr auto operator == (const Vec2&) const -> bool = default;

    [[nodiscard]] constexpr auto operator != (const Vec2& other) const -> bool {
        return !(*this == other);
    }

    // Arithmetic
    [[nodiscard]] constexpr auto operator - () const -> Vec2 {
        return {-x, -y};
    }

    [[nodiscard]] constexpr auto operator + (const Vec2& other) const -> Vec2 {
        return {x + other.x, y + other.y};
    }

    [[nodiscard]] constexpr auto operator - (const Vec2& other) const -> Vec2 {
        return {x - other.x, y - other.y};
    }

    [[nodiscard]] constexpr auto operator * (const Vec2& other) const -> Vec2 {
        return {x * other.x, y * other.y};
    }

    [[nodiscard]] constexpr auto operator / (const Vec2& other) const -> Vec2 {
        return {x / other.x, y / other.y};
    }

    [[nodiscard]] constexpr auto operator * (T scalar) const -> Vec2 {
        return {x * scalar, y * scalar};
    }

    [[nodiscard]] constexpr auto operator / (T scalar) const -> Vec2 {
        return {x / scalar, y / scalar};
    }

    constexpr auto operator += (const Vec2& other) -> Vec2& {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr auto operator -= (const Vec2& other) -> Vec2& {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr auto operator *= (const Vec2& other) -> Vec2& {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    constexpr auto operator /= (const Vec2& other) -> Vec2& {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    constexpr auto operator *= (T scalar) -> Vec2& {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr auto operator /= (T scalar) -> Vec2& {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Geometry
    [[nodiscard]] auto length() const -> double {
        return std::hypot(
            static_cast<double>(x),
            static_cast<double>(y)
        );
    }

    [[nodiscard]] constexpr auto squaredLength() const {
        return (x * x) + (y * y);
    }

    [[nodiscard]] auto distance(const Vec2& other) const -> double {
        return std::hypot(
            static_cast<double>(x - other.x),
            static_cast<double>(y - other.y)
        );
    }

    [[nodiscard]] constexpr auto squaredDistance(const Vec2& other) const {
        const auto dx = x - other.x;
        const auto dy = y - other.y;

        return (dx * dx) + (dy * dy);
    }

    [[nodiscard]] constexpr auto dot(const Vec2& other) const -> T {
        return (x * other.x) + (y * other.y);
    }

    /**
     * 2D cross product.
     *
     * Returns the scalar z-component of the 3D cross product.
     */
    [[nodiscard]] constexpr auto cross(const Vec2& other) const -> T {
        return (x * other.y) - (y * other.x);
    }

    /**
     * Returns the angle to another vector in degrees.
     */
    [[nodiscard]] auto angle(const Vec2& other) const -> double {
        const double denominator = length() * other.length();

        if (denominator == 0.0) {
            return 0.0;
        }

        double value = static_cast<double>(dot(other)) / denominator;
        value = std::clamp(value, -1.0, 1.0);

        return std::acos(value) * 180.0 / std::numbers::pi;
    }

    [[nodiscard]] constexpr auto flipped() const -> Vec2 {
        return {-x, -y};
    }

    [[nodiscard]] constexpr auto reflectedPoint(const Vec2& pivot) const -> Vec2 {
        return pivot + (pivot - *this);
    }

    [[nodiscard]] auto normalized() const -> Vec2 {
        Vec2 result = *this;
        result.normalize();
        return result;
    }

    void normalize() {
        const double l = length();

        if (l == 0.0) {
            return;
        }

        const double scale = 1.0 / l;

        x = static_cast<T>(x * scale);
        y = static_cast<T>(y * scale);
    }

    void setLength(T length) {
        const double current_length = this->length();

        if (current_length == 0.0) {
            return;
        }

        const double scale =
            static_cast<double>(length) / current_length;

        x = static_cast<T>(x * scale);
        y = static_cast<T>(y * scale);
    }

    [[nodiscard]] auto checkEqual(const Vec2& other, T threshold) const -> bool {
        return distance(other) <= static_cast<double>(threshold);
    }

    // Modification
    constexpr void set(T newX, T newY) {
        this->x = newX;
        this->y = newY;
    }

    constexpr void swap(Vec2& other) {
        std::swap(x, other.x);
        std::swap(y, other.y);
    }

    constexpr void zero() {
        x = T{};
        y = T{};
    }

    constexpr void flip() {
        x = -x;
        y = -y;
    }

    constexpr void translateX(T value) {
        x += value;
    }

    constexpr void translateY(T value) {
        y += value;
    }

    constexpr void translate(Vec2 t) {
        x += t.x;
        y += t.y;
    }

    constexpr void translate(T tx, T ty) {
        x += tx;
        y += ty;
    }

    constexpr void scaleX(T value) {
        x *= value;
    }

    constexpr void scaleY(T value) {
        y *= value;
    }

    constexpr void scale(T value) {
        x *= value;
        y *= value;
    }

    constexpr void scale(T sx, T sy) {
        x *= sx;
        y *= sy;
    }

    constexpr void scale(const Vec2<T>& pivot, T value) {
        x = pivot.x + ((x - pivot.x) * value);
        y = pivot.y + ((y - pivot.y) * value);
    }

    constexpr void scale(const Vec2<T>& pivot, T sx, T sy) {
        x = pivot.x + ((x - pivot.x) * sx);
        y = pivot.y + ((y - pivot.y) * sy);
    }

    void rotate(double radians) {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double new_x = (c * static_cast<double>(x)) - (s * static_cast<double>(y));
        const double new_y = (s * static_cast<double>(x)) + (c * static_cast<double>(y));

        x = static_cast<T>(new_x);
        y = static_cast<T>(new_y);
    }

    void rotateDegrees(double degrees) {
        rotate(degrees * std::numbers::pi / 180.0);
    }

    void rotate(const Vec2& pivot, double radians) {
        x -= pivot.x;
        y -= pivot.y;

        rotate(radians);

        x += pivot.x;
        y += pivot.y;
    }

    void rotateDegrees(const Vec2& pivot, double degrees) {
        rotate(pivot, degrees * std::numbers::pi / 180.0);
    }

    void setLerp(const Vec2& a, const Vec2& b, double t) {
        x = static_cast<T>(static_cast<double>(a.x) + (t * (static_cast<double>(b.x) - static_cast<double>(a.x))));
        y = static_cast<T>(static_cast<double>(a.y) + (t * (static_cast<double>(b.y) - static_cast<double>(a.y))));
    }

    [[nodiscard]] static auto lerp(const Vec2& a, const Vec2& b, double t) -> Vec2 {
        Vec2 result;
        result.setLerp(a, b, t);
        return result;
    }

    // CSV
    auto setByCSV(const char* csv, char delimiter = ',') -> bool {
        if (csv == nullptr) {
            return false;
        }
/*
        CSVLineParser parser(csv);
        parser.setDelimiter(delimiter);

        T values[2]{};
        const int32_t result = parser.values(2, values);

        if (result == 2) {
            x = values[0];
            y = values[1];
            return true;
        }
*/
        return false;
    }

    bool setByCSV(const String& csv, char delimiter = ',') {
        return setByCSV(csv.utf8(), delimiter);
    }


        /**
     *  @brief Performs linear interpolation towards another vector.
     *
     *  This method performs linear interpolation (lerp) towards another vector
     *  `v` based on the interpolation parameter `t`. Linear interpolation
     *  computes intermediate values between the currentMillis vector and `v` based on
     *  the blending factor `t`, where 0 represents the currentMillis vector and 1
     *  represents `v`. After calling this method, the currentMillis vector will be
     *  updated to be an interpolated value between its original state and `v`.
     *
     *  @param v The target vector to interpolate towards.
     *  @param t The interpolation parameter. Should be in the range [0, 1].
     */
    void lerpTowards(const Vec2& v, double t) noexcept {
        x += t * (v.x - x);
        y += t * (v.y - y);
    }


        /**
     *  @brief Monotonically increases with real angle, but doesn't need
     *         expensive trigonometry.
     *
     *  @return A value in [0, 1], increasing with angle, not the real angle.
     */
    [[nodiscard]] double pseudoAngle() const {
        const double p = static_cast<double>(x) / (std::abs(x) + std::abs(y));
        return (y > 0.0 ? 3.0 - p : 1.0 + p) / 4.0; // [0..1)
    }

    /**
     *  @brief Computes the angle in radians from this vector's position to
     *         another vector's position.
     *
     *  This function calculates the angle between the line from this vector to
     *  the vector `v` and the positive x-axis, using the atan2 function.
     *
     *  @param v The target vector to which the angle is measured.
     *  @return The angle in radians in the range [-π, π], representing the
     *          direction from this vector to `v`.
     */
    [[nodiscard]] double angleTo(const Vec2& v) const {
        return std::atan2(v.y - y, v.x - x);
    }

    /**
     *  @brief Computes the angle between this vector and another vector.
     *
     *  This function calculates the angle in radians between the currentMillis vector
     *  and the given vector `v`.
     *
     *  @param v The vector to which the angle is calculated.
     *  @return The angle in radians between this vector and the vector `v`.
     */
    [[nodiscard]] double angleRad(const Vec2& v) const noexcept {
        return std::atan2(cross(v), dot(v));
    }

    /**
     *  @brief Computes the signed angle in radians between two vectors.
     *
     *  This function calculates the angle between the vector and another vector
     *  `v`, with the result being:
     *  - Positive if the rotation to `v` is counterclockwise.
     *  - Negative if the rotation to `v` is clockwise.
     *
     *  The angle is measured in radians.
     *
     *  @param v The vector to which the rotation should be measured.
     *
     *  @return double The signed angle in radians between the two vectors.
     *
     *  @note Both vectors should not both be zero-length, as this will result
     *        in undefined behavior due to division by zero.
     *
     *  ### Edge Cases:
     *  - If either vector is a zero vector, the result is undefined (division
     *    by zero).
     *  - If the vectors are collinear, the result will be 0, PI or -PI.
     */
    [[nodiscard]] double signedAngleRad(const Vec2& v) noexcept {
        double dot = x * v.x + y * v.y;
        double length = std::sqrt((x * x + y * y) * (v.x * v.x + v.y * v.y));
        if (length == 0.0) {
            return 0.0;
        }
        double angle = std::acos(std::max<double>(-1.0, std::min<double>(1.0, dot / length)));
        if (x * v.y - y * v.x < 0) {
            angle = -angle;
        }
        return angle;
    }

    [[nodiscard]] static double signedAngleRad(T ux, T uy, T vx, T vy) noexcept {
        double dot = ux * vx + uy * vy;
        double length = std::sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
        if (length == 0.0) {
            return 0.0;
        }
        double angle = std::acos(std::max(-1.0, std::min(1.0, dot / length)));
        if (ux * vy - uy * vx < 0) {
            angle = -angle;
        }
        return angle;
    }

    /**
     *  @brief Computes the angle between vectors `a` and `b` with respect to
     *         this vector.
     *
     *  @param a The first vector.
     *  @param b The second vector.
     *  @return The angle in degrees between vector `a` and vector `b` relative
     *          to this vector.
     */
    [[nodiscard]] double angle(const Vec2& a, const Vec2& b) const noexcept {
        Vec2 va = a - *this;
        Vec2 vb = b - *this;
        return std::atan2(va.cross(vb), va.dot(vb)) / std::numbers::pi * 180.0;
    }

    /**
     *  @brief Computes the angle (in degrees) from the origin (0,0) to this
     *         point, measured clockwise from the positive Y-axis.
     *
     *  @return Angle in degrees in the range [0, 360].
     *          0 degrees points upward (+Y), 90 degrees points right (+X),
     *          180 degrees downward, 270 degrees left.
     */
    [[nodiscard]] double angleToPos() const noexcept {
        double angle = std::atan2(x, -y) * 180.0 / std::numbers::pi;
        if (angle < 0.0) angle += 360.0;
        return angle;
    }

    /**
     *  @brief Computes the angle (in degrees) from a given pivot point to this
     *         point, measured clockwise from the pivot's +Y-axis.
     *
     *  @param pivot The pivot position from which to measure the angle.
     *  @return Angle in degrees in the range [0, 360]. 0 degrees means the
     *          point lies directly above the pivot.
     */
    [[nodiscard]] double angleToPos(const Vec2& pivot) const noexcept {
        double angle = std::atan2(x - pivot.x, pivot.y - y) * 180.0 / std::numbers::pi;
        if (angle < 0.0) angle += 360.0;
        return angle;
    }



    /**
     *  @brief Checks if both components of the vector are valid numbers.
     *
     *  This method verifies whether both components of the vector are valid
     *  numbers. It returns true if both components are numeric values and false
     *  otherwise. This can be useful for checking if the vector has been
     *  properly initialized with valid numerical values.
     *
     *  @return `true` if both components are valid numbers, `false` otherwise.
     */
    [[nodiscard]] bool isNumber() const noexcept {
        return x == x && y == y;
    }

    /**
     *  @brief Checks if the vector is predominantly horizontal.
     *
     *  This method determines whether the vector is predominantly horizontal
     *  by comparing the absolute values of its x and y components. If the
     *  absolute value of the x component is greater than the absolute value
     *  of the y component, the vector is considered predominantly horizontal.
     *
     *  @return True if the vector is predominantly horizontal, false otherwise.
     */
    [[nodiscard]] bool isHorizontal() const noexcept {
        return std::fabs(x) > std::fabs(y);
    }

    /**
     *  @brief Checks if the vector is predominantly vertical.
     *
     *  This method determines whether the vector is predominantly vertical by
     *  comparing the absolute values of its x and y components. If the absolute
     *  value of the y component is greater than the absolute value of the x
     *  component, the vector is considered predominantly vertical.
     *
     *  @return `true` if the vector is predominantly vertical, `false`
     *          otherwise.
     */
    [[nodiscard]] bool isVertical() const noexcept {
        return std::fabs(y) > std::fabs(x);
    }

    [[nodiscard]] bool isOpposite(const Vec2& v) noexcept {
        return (x * v.x + y * v.y) < 0.0;
    }


    /**
     *  @brief Converts a location to a position.
     *
     *  Location has angle in degree an distance.
     *  Position has x and y.
     */
    [[nodiscard]] Vec2 locToPos() const noexcept {
        Vec2 result(0, y);
        result.rotate(-x);
        return result;
    }

    /**
     *  @brief Converts position to a location.
     */
    [[nodiscard]] Vec2 posToLoc() const noexcept {
        T distance = std::sqrt(x * x + y * y);
        T angle = distance > 0 ? (std::atan2(-x, -y) / std::numbers::pi) * 180 + 180 : 0;
        return Vec2(angle, distance);
    }


    /**
     *  @brief Determines if three 2D points form a clockwise orientation.
     *
     *  @param a First point
     *  @param b Second point
     *  @param c Third point
     *  @return true if the orientation of triangle abc is clockwise
     */
    [[nodiscard]] static inline bool isClockwise(const Vec2& a, const Vec2& b, const Vec2& c) noexcept {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) < 0.0;
    }

    /**
     *  @brief Checks whether the vector lies inside the circumcircle
     *         of the triangle defined by `a`, `b`, and `c`.
     *
     * @param a First vertex of the triangle.
     * @param b Second vertex of the triangle.
     * @param c Third vertex of the triangle.
     * @return true if vector lies inside the circumcircle of triangle abc,
     *         false otherwise.
     */
    [[nodiscard]] bool inCircle(const Vec2& a, const Vec2& b, const Vec2& c) const noexcept {
        const double dx = a.x - x;
        const double dy = a.y - y;
        const double ex = b.x - x;
        const double ey = b.y - y;
        const double fx = c.x - x;
        const double fy = c.y - y;

        const double ap = dx * dx + dy * dy;
        const double bp = ex * ex + ey * ey;
        const double cp = fx * fx + fy * fy;

        return (dx * (ey * cp - bp * fy) -
                dy * (ex * cp - bp * fx) +
                ap * (ex * fy - ey * fx)) < 0.0;
    }

    /**
     *  @brief Set the vector to be a the center of the circle that passes
     *         through three given points.
     *
     *  @param a A Vec2 representing the first point on the circle.
     *  @param b A Vec2 representing the second point on the circle.
     *  @param c A Vec2 representing the third point on the circle.
     */
    void circumcenter(const Vec2& a, const Vec2& b, const Vec2& c) {
        const double dx = b.x - a.x;
        const double dy = b.y - a.y;
        const double ex = c.x - a.x;
        const double ey = c.y - a.y;

        const double bl = dx * dx + dy * dy;
        const double cl = ex * ex + ey * ey;
        const double d = dx * ey - dy * ex;

        x = a.x + (ey * bl - dy * cl) * 0.5 / d;
        y = a.y + (dx * cl - ex * bl) * 0.5 / d;
    }

    /**
     *  @brief Computes the squared radius of the circumcircle of the triangle
     *         defined by three 2D points.
     *
     *  This function calculates the square of the radius of the unique circle
     *  (circumcircle) that passes through the three given points `a`, `b`, and
     *  `c`. It avoids computing the square root for performance and returns a
     *  large value (`std::numeric_limits<double>::max()`) if the triangle is
     *  degenerate (i.e., points are colinear or overlapping).
     *
     *  @param a The first vertex of the triangle.
     *  @param b The second vertex of the triangle.
     *  @param c The third vertex of the triangle.
     *  @return The squared radius of the circumcircle, or a large fallback
     *          value if invalid.
     */
    [[nodiscard]] static double squaredCircumradius(const Vec2& a, const Vec2& b, const Vec2& c) {
        const Vec2 d = b - a;
        const Vec2 e = c - a;

        const double bl = d.squaredLength();
        const double cl = e.squaredLength();
        const double dc = d.cross(e);

        if (bl != 0.0 && cl != 0.0 && dc != 0.0) {
            const double x = (e.y * bl - d.y * cl) * 0.5 / dc;
            const double y = (d.x * cl - e.x * bl) * 0.5 / dc;
            return x * x + y * y;
        }
        else {
            return std::numeric_limits<double>::max();
        }
    }

    /**
     *  @brief Set the vector to be a point on an arc defined by three points.
     *
     *  This function computes a point on a circular arc defined by three points:
     *  `a`, `b`, and `c`. The parameter `t` is a value between 0 and 1,
     *  representing the position along the arc:
     *  - `t = 0` gives the point `a`
     *  - `t = 1` gives the point `c`
     *  - `t = 0.5` gives the midpoint of the arc between `a` and `c`, passing through `b`
     *
     *  The arc is assumed to be part of a circle passing through the three given points.
     *  The function will update the currentMillis vector (the object the method is called on) to
     *  represent a point along this arc based on the parameter `t`.
     *
     *  @param a The first point on the arc (start point of the arc).
     *  @param b The second point on the arc, used to define the curvature (typically the apex).
     *  @param c The third point on the arc (end point of the arc).
     *  @param t A parameter between 0 and 1 that specifies the position on the arc:
     *           - 0 means the point is at `a`
     *           - 1 means the point is at `c`
     *           - 0.5 gives the midpoint of the arc.
     */
    void pointOnArc(const Vec2& a, const Vec2& b, const Vec2& c, double t) {
        // Calculate the center of the circle
        Vec2 center;
        center.circumcenter(a, b, c);

        // Calculate the radius of the circle
        double radius = std::hypot(center.x - a.x, center.y - a.y);

        // Calculate angles from center to points `a`, `b`, and `c`
        double angle_ca = std::atan2(a.y - center.y, a.x - center.x);
        double angle_cc = std::atan2(c.y - center.y, c.x - center.x);

        // Calculate the total angle (taking care of direction).
        double total_angle = angle_cc - angle_ca;
        if (total_angle < 0.0) {
            total_angle += 2.0 * std::numbers::pi;
        }

        // Calculate the angle at parameter `t`.
        double angle_at_t = angle_ca + total_angle * t;

        // Calculate the point on the arc using polar coordinates.
        x = center.x + radius * std::cos(angle_at_t);
        y = center.y + radius * std::sin(angle_at_t);
    }

    void pointOnArc(const Vec2* p, double t) {
        if (p) {
            pointOnArc(p[0], p[1], p[2], t);
        }
    }

    /**
     *  @brief Computes the area of a simple 2D polygon using the shoelace
     *         formula.
     *
     *  This function calculates the absolute area of a polygon defined by an
     *  array of points. The polygon is assumed to be simple
     *  (non-self-intersecting) and the vertices should be ordered either
     *  clockwise or counterclockwise.
     *
     *  @param point_n The number of points (vertices) in the polygon.
     *  @param points A pointer to an array of Vec2<T> representing
     *                the polygon's vertices.
     *  @return The absolute area of the polygon.
     */
    [[nodiscard]] static double polygonArea(int32_t point_n, const Vec2<T>* points) noexcept {
        if (point_n < 3) {
            return 0.0;  // Not a polygon
        }

        double area = 0.0;
        for (int32_t i = 0; i < point_n; ++i) {
            const Vec2& v0 = points[i];
            const Vec2& v1 = points[(i + 1) % point_n];
            area += v0.x * v1.y - v1.x * v0.y;
        }
        return std::abs(area) * 0.5;
    }

    /**
     *  @brief Computes the centroid of a polygon.
     *
     *  This function calculates the centroid (center of mass) of a simple,
     *  non-self-intersecting polygon.
     *
     *  @param point_n The number of vertices in the polygon.
     *  @param points A pointer to an array of Vec2<T> representing the polygon's vertices.
     *  @return The centroid as a Vec2<double>.
     */
    [[nodiscard]] static Vec2<double> polygonCentroid(int32_t point_n, const Vec2<T>* points) noexcept {
        double cx = 0.0, cy = 0.0;
        double area = 0.0;

        for (int32_t i = 0; i < point_n; ++i) {
            const Vec2& v0 = points[i];
            const Vec2& v1 = points[(i + 1) % point_n];
            double cross = v0.x * v1.y - v1.x * v0.y;
            cx += (v0.x + v1.x) * cross;
            cy += (v0.y + v1.y) * cross;
            area += cross;
        }

        area *= 0.5;
        if (std::abs(area) < 1e-10) {
            // Degenerate case (area ≈ 0), return average of points
            double avg_x = 0.0, avg_y = 0.0;
            for (int32_t i = 0; i < point_n; ++i) {
                avg_x += points[i].x;
                avg_y += points[i].y;
            }
            return { avg_x / point_n, avg_y / point_n };
        }

        cx /= (6.0 * area);
        cy /= (6.0 * area);

        return { cx, cy };
    }


    // Serialization
    void writeToFile(File& file);
    void readFromFile(File& file);
};

// Scalar multiplication from the left.
template <typename T>
[[nodiscard]] constexpr auto operator * (T scalar, const Vec2<T>& vector) -> Vec2<T> {
    return vector * scalar;
}

// Stream output.
template <typename T>
auto operator << (std::ostream& os, const Vec2<T>& vector) -> std::ostream& {
    if constexpr (
        std::is_same_v<T, int8_t> ||
        std::is_same_v<T, uint8_t>
    ) {
        return os
            << static_cast<int32_t>(vector.x)
            << ", "
            << static_cast<int32_t>(vector.y);
    }

    return os << vector.x << ", " << vector.y;
}

// Standard types.
using Vec2i = Vec2<int32_t>;
using Vec2l = Vec2<int64_t>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

} // namespace Grain