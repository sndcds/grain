
#pragma once

/*
    Implementation based on:
    https://github.com/delfrrr/delaunator-cpp/blob/master/include/delaunator.hpp
*/

#include <grain/Container/List.hpp>
#include <grain/Math/Vec2.hpp>
#include <grain/Geometry/Geometry.hpp>
#include <grain/Geometry/Rect.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>


namespace Grain {


namespace detail {

//------------------------------------------------------------------------------
//  Neumaier summation
//------------------------------------------------------------------------------

/**
 * Compensated summation using the Neumaier variant.
 *
 * Reduces floating-point accumulation errors.
 */
static auto compensatedSum(const std::vector<double>& values) -> double {

    if (values.empty()) {
        return 0.0;
    }

    double result = 0.0;
    double error = 0.0;

    for (const double value : values) {

        const double next = result + value;

        if (std::abs(result) >= std::abs(value)) {
            error += (result - next) + value;
        }
        else {
            error += (value - next) + result;
        }

        result = next;
    }

    return result + error;
}

} // namespace detail


//------------------------------------------------------------------------------
//  Delaunay
//------------------------------------------------------------------------------

template<typename T, typename U>
class Delaunay {

public:

    static constexpr T kInvalidIndex = std::numeric_limits<T>::max();


    //--------------------------------------------------------------------------
    //  Compare
    //--------------------------------------------------------------------------

    struct Compare {

        const List<Vec2<U>>& vertex_list;
        Vec2<U> pos;

        auto operator()(T a, T b) const -> bool {

            const Vec2<U>& a_pos = vertex_list[a];
            const Vec2<U>& b_pos = vertex_list[b];

            const double d1 = pos.squaredDistance(a_pos);
            const double d2 = pos.squaredDistance(b_pos);

            if (d1 != d2) {
                return d1 < d2;
            }

            if (a_pos.x != b_pos.x) {
                return a_pos.x < b_pos.x;
            }

            return a_pos.y < b_pos.y;
        }
    };


    //--------------------------------------------------------------------------
    //  AngleCenter
    //--------------------------------------------------------------------------

    struct AngleCenter {
        U angle;
        Vec2<U> center;
    };


public:

    //--------------------------------------------------------------------------
    //  Public data
    //--------------------------------------------------------------------------

    List<T> halfedges_;
    List<T> hull_prev_;
    List<T> hull_next_;
    List<T> hull_tri_;

    T hull_start_ = 0;

    // If true, the Y axis points downwards.
    bool flip_y_ = true;

    Rect<U> bounds_;

    bool clip_at_bounds_flag_ = false;
    bool invalid_bounds_flag_ = false;


private:

    //--------------------------------------------------------------------------
    //  Internal data
    //--------------------------------------------------------------------------

    const List<Vec2<U>>& vertex_list_;

    List<T> triangle_indices_;
    List<T> hash_;

    Vec2<U> center_;

    T hash_size_ = 0;

    List<T> edge_stack_;

    std::vector<std::vector<Vec2<U>>> voronoi_cells_;

public:

    //--------------------------------------------------------------------------
    //  Construction
    //--------------------------------------------------------------------------

    explicit Delaunay(const List<Vec2<U>>& vertex_list)
        : vertex_list_(vertex_list) {
    }


    Delaunay(const Delaunay&) = delete;
    auto operator = (const Delaunay&) -> Delaunay& = delete;

    Delaunay(Delaunay&&) = delete;
    auto operator = (Delaunay&&) -> Delaunay& = delete;


    //--------------------------------------------------------------------------
    //  Triangulation
    //--------------------------------------------------------------------------

    auto setBounds(const Rect<U>& bounds) -> void;

    auto update() -> void;

    auto hullArea() -> double;


    //--------------------------------------------------------------------------
    //  Triangle access
    //--------------------------------------------------------------------------

    auto triangleVertexCount() const noexcept -> T {
        return static_cast<T>(vertex_list_.size());
    }

    auto triangleVertexList() const noexcept -> const List<Vec2<U>>& {
        return vertex_list_;
    }

    auto triangleIndexList() const noexcept -> const List<T>& {
        return triangle_indices_;
    }

    auto triangleVertexAtIndex(T index) const noexcept -> const Vec2<U>& {
        return vertex_list_[triangle_indices_[index]];
    }


    //--------------------------------------------------------------------------
    //  Voronoi access
    //--------------------------------------------------------------------------

    auto voronoiCellCount() const noexcept -> T {
        return static_cast<T>(voronoi_cells_.size());
    }

    auto voronoiCells() const noexcept
        -> const std::vector<std::vector<Vec2<U>>>& {
        return voronoi_cells_;
    }

    auto voronoiCellSizes() const noexcept -> std::vector<T> {
        std::vector<T> sizes;
        sizes.reserve(voronoi_cells_.size());

        for (const auto& cell : voronoi_cells_) {
            sizes.push_back(static_cast<T>(cell.size()));
        }

        return sizes;
    }

    auto computeVoronoiCells() noexcept -> void;


    //--------------------------------------------------------------------------
    //  Polygon clipping
    //--------------------------------------------------------------------------

    auto inside(
        const Vec2<U>& v,
        Edge edge
    ) -> bool;

    auto intersect(
        const Vec2<U>& a,
        const Vec2<U>& b,
        Edge edge
    ) -> Vec2<U>;

    auto clipAgainstEdge(
        const Vec2<U>* vertices,
        T vertex_count,
        Edge edge,
        Vec2<U>* out_vertices
    ) -> T;

    auto clipPolygonToRect(
        const Vec2<U>* vertices,
        T vertex_count,
        Vec2<U>* out_vertices
    ) -> T;


private:

    //--------------------------------------------------------------------------
    //  Internal triangulation
    //--------------------------------------------------------------------------

    auto legalize(T a) -> T;

    auto hashKey(const Vec2<U>& pos) const -> T;

    auto addTriangle(
        T i0,
        T i1,
        T i2,
        T a,
        T b,
        T c
    ) -> T;

    auto link(T a, T b) -> void;

    auto isClockwise(
        const Vec2<U>& a,
        const Vec2<U>& b,
        const Vec2<U>& c
    ) -> bool;

    static auto fastMod(T i, T c) -> T;
};


//------------------------------------------------------------------------------
//  Standard types
//------------------------------------------------------------------------------

using Delaunay64d = Delaunay<int64_t, double>;
using Delaunay32d = Delaunay<int32_t, double>;
using Delaunay64f = Delaunay<int64_t, float>;
using Delaunay32f = Delaunay<int32_t, float>;


} // namespace Grain
