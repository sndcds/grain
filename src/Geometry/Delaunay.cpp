#include <grain/Geometry/Delaunay.hpp>
#include <grain/Geometry/Bounds.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>


namespace Grain {


//------------------------------------------------------------------------------
//  Bounds
//------------------------------------------------------------------------------

template<typename T, typename U>
void Delaunay<T, U>::setBounds(const Rect<U>& bounds) {
    bounds_ = bounds;
    invalid_bounds_flag_ = !bounds_.isValid();
    clip_at_bounds_flag_ = true;
}


//------------------------------------------------------------------------------
//  Triangulation
//------------------------------------------------------------------------------

template<typename T, typename U>
void Delaunay<T, U>::update() {

    const T n = static_cast<T>(vertex_list_.size());

    // Reset previous triangulation.
    halfedges_.clear();
    hull_prev_.clear();
    hull_next_.clear();
    hull_tri_.clear();
    triangle_indices_.clear();
    hash_.clear();
    edge_stack_.clear();

    hull_start_ = kInvalidIndex;
    hash_size_ = 0;

    if (n < 3) {
        return;
    }

    // Calculate the bounding range.
    Bounds<U> range;
    range.initForMinMaxSearch();

    List<T> ids;
    ids.reserve(n);

    T vertex_index = 0;

    for (const auto& vertex : vertex_list_) {
        range.add(vertex);
        ids.push_back(vertex_index++);
    }

    const Vec2<U> centroid = range.center();

    // Find the vertex closest to the centroid.
    double min_dist = std::numeric_limits<double>::max();

    T i0 = kInvalidIndex;
    T i1 = kInvalidIndex;
    T i2 = kInvalidIndex;

    for (T i = 0; i < n; ++i) {

        const double distance = centroid.distance(vertex_list_[i]);

        if (distance < min_dist) {
            i0 = i;
            min_dist = distance;
        }
    }

    if (i0 == kInvalidIndex) {
        return;
    }

    const Vec2<U> i0_pos = vertex_list_[i0];

    // Find the closest distinct vertex.
    min_dist = std::numeric_limits<double>::max();

    for (T i = 0; i < n; ++i) {

        if (i == i0) {
            continue;
        }

        const double distance = i0_pos.distance(vertex_list_[i]);

        if (distance > 0.0 && distance < min_dist) {
            i1 = i;
            min_dist = distance;
        }
    }

    if (i1 == kInvalidIndex) {
        return;
    }

    Vec2<U> i1_pos = vertex_list_[i1];

    // Find the third vertex forming the smallest circumcircle.
    double min_radius = std::numeric_limits<double>::max();

    for (T i = 0; i < n; ++i) {

        if (i == i0 || i == i1) {
            continue;
        }

        const double radius = Vec2<U>::squaredCircumradius(
            i0_pos,
            i1_pos,
            vertex_list_[i]
        );

        if (radius < min_radius) {
            i2 = i;
            min_radius = radius;
        }
    }

    if (i2 == kInvalidIndex ||
        !(min_radius < std::numeric_limits<double>::max())) {
        throw std::runtime_error("Delaunay: Cannot find initial triangle");
    }

    Vec2<U> i2_pos = vertex_list_[i2];

    // Ensure clockwise orientation.
    if (isClockwise(i0_pos, i1_pos, i2_pos)) {
        std::swap(i1, i2);
        std::swap(i1_pos, i2_pos);
    }

    // Calculate circumcenter.
    center_.circumcenter(i0_pos, i1_pos, i2_pos);

    // Sort vertices by distance from the circumcenter.
    std::sort(
        ids.begin(),
        ids.end(),
        Compare{vertex_list_, center_}
    );

    // Initialize the edge hash.
    hash_size_ = static_cast<T>(
        std::llround(std::ceil(std::sqrt(static_cast<double>(n))))
    );

    if (hash_size_ <= 0) {
        return;
    }

    hash_.reserve(hash_size_);

    // Initialize convex hull arrays.
    hull_prev_.reserve(n);
    hull_next_.reserve(n);
    hull_tri_.reserve(n);

    hull_start_ = i0;

    T hull_size = 3;

    hull_next_[i0] = hull_prev_[i2] = i1;
    hull_next_[i1] = hull_prev_[i0] = i2;
    hull_next_[i2] = hull_prev_[i1] = i0;

    hull_tri_[i0] = 0;
    hull_tri_[i1] = 1;
    hull_tri_[i2] = 2;

    hash_[hashKey(i0_pos)] = i0;
    hash_[hashKey(i1_pos)] = i1;
    hash_[hashKey(i2_pos)] = i2;

    const T max_triangles = 2 * n - 5;

    triangle_indices_.reserve(max_triangles * 3);
    halfedges_.reserve(max_triangles * 3);

    addTriangle(
        i0,
        i1,
        i2,
        kInvalidIndex,
        kInvalidIndex,
        kInvalidIndex
    );

    const double epsilon = std::numeric_limits<double>::epsilon();

    Vec2<U> previous_pos(
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::quiet_NaN()
    );

    // Insert remaining vertices.
    for (T k = 0; k < n; ++k) {

        const T i = ids[k];
        const Vec2<U> pos = vertex_list_[i];

        // Skip near-duplicate points.
        if (k > 0 && pos.checkEqual(previous_pos, epsilon)) {
            continue;
        }

        previous_pos = pos;

        // Skip the initial triangle vertices.
        if (pos.checkEqual(i0_pos, epsilon) ||
            pos.checkEqual(i1_pos, epsilon) ||
            pos.checkEqual(i2_pos, epsilon)) {
            continue;
        }

        // Find a visible edge using the hull hash.
        const T key = hashKey(pos);

        T start = kInvalidIndex;

        for (T j = 0; j < hash_size_; ++j) {

            const T candidate = hash_[fastMod(key + j, hash_size_)];

            if (candidate != kInvalidIndex &&
                hull_next_[candidate] != candidate) {

                start = candidate;
                break;
            }
        }

        if (start == kInvalidIndex) {
            continue;
        }

        start = hull_prev_[start];

        T e = start;
        T q = kInvalidIndex;

        // Find the first visible hull edge.
        while (true) {

            q = hull_next_[e];

            if (isClockwise(pos, vertex_list_[e], vertex_list_[q])) {
                break;
            }

            e = q;

            if (e == start) {
                e = kInvalidIndex;
                break;
            }
        }

        if (e == kInvalidIndex) {
            continue;
        }

        // Add the first triangle.
        T t = addTriangle(
            e,
            i,
            hull_next_[e],
            kInvalidIndex,
            kInvalidIndex,
            hull_tri_[e]
        );

        hull_tri_[i] = legalize(t + 2);
        hull_tri_[e] = t;

        ++hull_size;

        // Walk forward through the hull.
        T next = hull_next_[e];

        while (true) {

            q = hull_next_[next];

            if (!isClockwise(pos, vertex_list_[next], vertex_list_[q])) {
                break;
            }

            t = addTriangle(
                next,
                i,
                q,
                hull_tri_[i],
                kInvalidIndex,
                hull_tri_[next]
            );

            hull_tri_[i] = legalize(t + 2);

            // Mark the removed hull vertex.
            hull_next_[next] = next;

            --hull_size;
            next = q;
        }

        // Walk backward through the hull.
        if (e == start) {

            while (true) {

                q = hull_prev_[e];

                if (!isClockwise(pos, vertex_list_[q], vertex_list_[e])) {
                    break;
                }

                t = addTriangle(
                    q,
                    i,
                    e,
                    kInvalidIndex,
                    hull_tri_[e],
                    hull_tri_[q]
                );

                legalize(t + 2);

                hull_tri_[q] = t;

                // Mark the removed hull vertex.
                hull_next_[e] = e;

                --hull_size;
                e = q;
            }
        }

        // Update hull indices.
        hull_prev_[i] = e;
        hull_start_ = e;

        hull_prev_[next] = i;
        hull_next_[e] = i;
        hull_next_[i] = next;

        // Update the hash.
        hash_[hashKey(pos)] = i;
        hash_[hashKey(vertex_list_[e])] = e;
    }

    (void)hull_size;
}


//------------------------------------------------------------------------------
//  Convex hull area
//------------------------------------------------------------------------------

template<typename T, typename U>
double Delaunay<T, U>::hullArea() {

    if (hull_start_ == kInvalidIndex || hull_next_.empty()) {
        return 0.0;
    }

    double area = 0.0;

    T e = hull_start_;

    do {

        const Vec2<U>& current = vertex_list_[e];
        const Vec2<U>& previous = vertex_list_[hull_prev_[e]];

        area +=
            (static_cast<double>(current.x) - previous.x) *
            (static_cast<double>(current.y) + previous.y);

        e = hull_next_[e];

    } while (e != hull_start_);

    return area * 0.5;
}


//------------------------------------------------------------------------------
//  Voronoi cells
//------------------------------------------------------------------------------


template<typename T, typename U>
void Delaunay<T, U>::computeVoronoiCells() noexcept {

    voronoi_cells_.clear();

    try {

        const size_t point_count = vertex_list_.size();

        if (point_count == 0 ||
            !clip_at_bounds_flag_ ||
            invalid_bounds_flag_) {
            return;
        }

        const double left = static_cast<double>(bounds_.x);
        const double top = static_cast<double>(bounds_.y);
        const double right = left + static_cast<double>(bounds_.width);
        const double bottom = top + static_cast<double>(bounds_.height);

        if (!(right > left && bottom > top)) {
            return;
        }

        voronoi_cells_.reserve(point_count);

        // Construct one Voronoi cell per seed.
        for (size_t i = 0; i < point_count; ++i) {

            const Vec2<U>& seed = vertex_list_[i];

            // Start with the complete bounding rectangle.
            std::vector<Vec2<U>> polygon = {
                Vec2<U>(static_cast<U>(left),  static_cast<U>(top)),
                Vec2<U>(static_cast<U>(right), static_cast<U>(top)),
                Vec2<U>(static_cast<U>(right), static_cast<U>(bottom)),
                Vec2<U>(static_cast<U>(left),  static_cast<U>(bottom))
            };

            // Clip against the bisector of every other seed.
            for (size_t j = 0; j < point_count; ++j) {

                if (i == j || polygon.empty()) {
                    continue;
                }

                const Vec2<U>& other = vertex_list_[j];

                const double dx =
                    static_cast<double>(other.x) -
                    static_cast<double>(seed.x);

                const double dy =
                    static_cast<double>(other.y) -
                    static_cast<double>(seed.y);

                // Identical seeds do not define a bisector.
                if (dx == 0.0 && dy == 0.0) {
                    continue;
                }

                // Perpendicular bisector.
                const double mid_x =
                    (static_cast<double>(seed.x) +
                     static_cast<double>(other.x)) * 0.5;

                const double mid_y =
                    (static_cast<double>(seed.y) +
                     static_cast<double>(other.y)) * 0.5;

                // A point belongs to this half-plane if it is
                // at least as close to seed as it is to other.
                auto signedDistance = [&](const Vec2<U>& p) -> double {

                    return
                        (static_cast<double>(p.x) - mid_x) * dx +
                        (static_cast<double>(p.y) - mid_y) * dy;
                };

                std::vector<Vec2<U>> clipped;
                clipped.reserve(polygon.size() + 1);

                Vec2<U> previous = polygon.back();
                double previous_distance = signedDistance(previous);

                bool previous_inside = previous_distance <= 0.0;

                for (const auto& current : polygon) {

                    const double current_distance =
                        signedDistance(current);

                    const bool current_inside =
                        current_distance <= 0.0;

                    if (current_inside != previous_inside) {

                        const double denominator =
                            previous_distance - current_distance;

                        if (denominator != 0.0) {

                            const double t =
                                previous_distance / denominator;

                            clipped.emplace_back(
                                static_cast<U>(
                                    static_cast<double>(previous.x) +
                                    t * (static_cast<double>(current.x) -
                                         static_cast<double>(previous.x))
                                ),
                                static_cast<U>(
                                    static_cast<double>(previous.y) +
                                    t * (static_cast<double>(current.y) -
                                         static_cast<double>(previous.y))
                                )
                            );
                        }
                    }

                    if (current_inside) {
                        clipped.push_back(current);
                    }

                    previous = current;
                    previous_distance = current_distance;
                    previous_inside = current_inside;
                }

                polygon = std::move(clipped);
            }

            if (polygon.size() >= 3) {
                voronoi_cells_.push_back(std::move(polygon));
            }
        }
    }
    catch (...) {
        // This function is noexcept.
        voronoi_cells_.clear();
    }
}



//------------------------------------------------------------------------------
//  Polygon clipping
//------------------------------------------------------------------------------

template<typename T, typename U>
bool Delaunay<T, U>::inside(const Vec2<U>& v, Edge edge) {

    switch (edge) {

        case Edge::Left:
            return v.x >= bounds_.x;

        case Edge::Right:
            return v.x <= bounds_.x + bounds_.width;

        case Edge::Top:
            return v.y >= bounds_.y;

        case Edge::Bottom:
            return v.y <= bounds_.y + bounds_.height;

        default:
            return false;
    }
}


template<typename T, typename U>
Vec2<U> Delaunay<T, U>::intersect(
    const Vec2<U>& a,
    const Vec2<U>& b,
    Edge edge
) {

    const double dx = static_cast<double>(b.x) - a.x;
    const double dy = static_cast<double>(b.y) - a.y;

    switch (edge) {

        case Edge::Left: {

            if (dx == 0.0) {
                return a;
            }

            const double x = bounds_.x;
            const double t = (x - a.x) / dx;

            return Vec2<U>(
                x,
                a.y + t * dy
            );
        }

        case Edge::Right: {

            if (dx == 0.0) {
                return a;
            }

            const double x = bounds_.x + bounds_.width;
            const double t = (x - a.x) / dx;

            return Vec2<U>(
                x,
                a.y + t * dy
            );
        }

        case Edge::Top: {

            if (dy == 0.0) {
                return a;
            }

            const double y = bounds_.y;
            const double t = (y - a.y) / dy;

            return Vec2<U>(
                a.x + t * dx,
                y
            );
        }

        case Edge::Bottom: {

            if (dy == 0.0) {
                return a;
            }

            const double y = bounds_.y + bounds_.height;
            const double t = (y - a.y) / dy;

            return Vec2<U>(
                a.x + t * dx,
                y
            );
        }
    }

    return Vec2<U>();
}


template<typename T, typename U>
T Delaunay<T, U>::clipAgainstEdge(
    const Vec2<U>* vertices,
    T vertex_n,
    Edge edge,
    Vec2<U>* out_vertices
) {

    if (vertices == nullptr ||
        out_vertices == nullptr ||
        vertex_n <= 0) {
        return 0;
    }

    T out_count = 0;

    Vec2<U> previous = vertices[vertex_n - 1];
    bool previous_inside = inside(previous, edge);

    for (T i = 0; i < vertex_n; ++i) {

        const Vec2<U>& current = vertices[i];
        const bool current_inside = inside(current, edge);

        if (current_inside) {

            if (!previous_inside) {
                out_vertices[out_count++] = intersect(previous, current, edge);
            }

            out_vertices[out_count++] = current;
        }
        else if (previous_inside) {

            out_vertices[out_count++] = intersect(previous, current, edge);
        }

        previous = current;
        previous_inside = current_inside;
    }

    return out_count;
}


template<typename T, typename U>
T Delaunay<T, U>::clipPolygonToRect(
    const Vec2<U>* vertices,
    T vertex_n,
    Vec2<U>* out_vertices
) {

    if (vertices == nullptr ||
        out_vertices == nullptr ||
        vertex_n <= 0 ||
        invalid_bounds_flag_) {
        return 0;
    }

    try {

        std::vector<Vec2<U>> polygon(
            vertices,
            vertices + static_cast<size_t>(vertex_n)
        );

        // Clip against all four rectangle edges.
        for (const Edge edge : {
            Edge::Left,
            Edge::Right,
            Edge::Top,
            Edge::Bottom
        }) {

            if (polygon.empty()) {
                break;
            }

            std::vector<Vec2<U>> clipped;
            clipped.reserve(polygon.size() + 4);

            Vec2<U> previous = polygon.back();
            bool previous_inside = inside(previous, edge);

            for (const auto& current : polygon) {

                const bool current_inside = inside(current, edge);

                if (current_inside) {

                    if (!previous_inside) {
                        clipped.push_back(
                            intersect(previous, current, edge)
                        );
                    }

                    clipped.push_back(current);
                }
                else if (previous_inside) {

                    clipped.push_back(
                        intersect(previous, current, edge)
                    );
                }

                previous = current;
                previous_inside = current_inside;
            }

            polygon = std::move(clipped);
        }

        std::copy(
            polygon.begin(),
            polygon.end(),
            out_vertices
        );

        return static_cast<T>(polygon.size());
    }
    catch (...) {
        return 0;
    }
}


//------------------------------------------------------------------------------
//  Edge legalization
//------------------------------------------------------------------------------

template<typename T, typename U>
T Delaunay<T, U>::legalize(T a) {

    T i = 0;
    T ar = 0;

    edge_stack_.clear();

    // Recursion eliminated with an explicit stack.
    while (true) {

        const T b = halfedges_[a];

        /*
         * If the pair of triangles doesn't satisfy the Delaunay condition,
         * flip the shared edge and continue checking the affected edges.
         *
         *           pl                    pl
         *          /||\                  /  \
         *       al/ || \bl            al/    \a
         *        /  ||  \              /      \
         *       /  a||b  \    flip    /___ar___\
         *     p0\   ||   /p1   =>   p0\---bl---/p1
         *        \  ||  /              \      /
         *       ar\ || /br             b\    /br
         *          \||/                  \  /
         *           pr                    pr
         */

        const T a0 = 3 * (a / 3);
        ar = a0 + (a + 2) % 3;

        if (b == kInvalidIndex) {

            if (i > 0) {
                --i;
                a = edge_stack_[i];
                continue;
            }

            break;
        }

        const T b0 = 3 * (b / 3);

        const T al = a0 + (a + 1) % 3;
        const T bl = b0 + (b + 2) % 3;

        const T p0 = triangle_indices_[ar];
        const T pr = triangle_indices_[a];
        const T pl = triangle_indices_[al];
        const T p1 = triangle_indices_[bl];

        const bool illegal = vertex_list_[p1].inCircle(
            vertex_list_[p0],
            vertex_list_[pr],
            vertex_list_[pl]
        );

        if (illegal) {

            triangle_indices_[a] = p1;
            triangle_indices_[b] = p0;

            const T hbl = halfedges_[bl];

            // Edge swapped on the other side of the hull.
            if (hbl == kInvalidIndex) {

                T e = hull_start_;

                do {

                    if (hull_tri_[e] == bl) {
                        hull_tri_[e] = a;
                        break;
                    }

                    e = hull_next_[e];

                } while (e != hull_start_);
            }

            link(a, hbl);
            link(b, halfedges_[ar]);
            link(ar, bl);

            const T br = b0 + (b + 1) % 3;

            if (i < static_cast<T>(edge_stack_.size())) {
                edge_stack_[i] = br;
            }
            else {
                edge_stack_.push_back(br);
            }

            ++i;
        }
        else {

            if (i > 0) {
                --i;
                a = edge_stack_[i];
                continue;
            }

            break;
        }
    }

    return ar;
}


//------------------------------------------------------------------------------
//  Edge hash
//------------------------------------------------------------------------------

template<typename T, typename U>
T Delaunay<T, U>::hashKey(const Vec2<U>& pos) const {

    const Vec2<U> d = pos - center_;

    return fastMod(
        static_cast<T>(
            std::llround(
                std::floor(
                    d.pseudoAngle() * static_cast<double>(hash_size_)
                )
            )
        ),
        hash_size_
    );
}


//------------------------------------------------------------------------------
//  Triangle construction
//------------------------------------------------------------------------------

template<typename T, typename U>
T Delaunay<T, U>::addTriangle(
    T i0,
    T i1,
    T i2,
    T a,
    T b,
    T c
) {

    const T t = static_cast<T>(triangle_indices_.size());

    triangle_indices_.push_back(i0);
    triangle_indices_.push_back(i1);
    triangle_indices_.push_back(i2);

    link(t, a);
    link(t + 1, b);
    link(t + 2, c);

    return t;
}


//------------------------------------------------------------------------------
//  Half-edge linking
//------------------------------------------------------------------------------

template<typename T, typename U>
void Delaunay<T, U>::link(const T a, const T b) {

    const T size = static_cast<T>(halfedges_.size());

    if (a == size) {
        halfedges_.push_back(b);
    }
    else if (a < size) {
        halfedges_[a] = b;
    }
    else {
        throw std::runtime_error("Delaunay: Cannot link edge");
    }

    if (b != kInvalidIndex) {

        const T size2 = static_cast<T>(halfedges_.size());

        if (b == size2) {
            halfedges_.push_back(a);
        }
        else if (b < size2) {
            halfedges_[b] = a;
        }
        else {
            throw std::runtime_error("Delaunay: Cannot link edge");
        }
    }
}


//------------------------------------------------------------------------------
//  Orientation
//------------------------------------------------------------------------------

template<typename T, typename U>
bool Delaunay<T, U>::isClockwise(
    const Vec2<U>& a,
    const Vec2<U>& b,
    const Vec2<U>& c
) {

    const bool result = Vec2<U>::isClockwise(a, b, c);

    return flip_y_ ? !result : result;
}


//------------------------------------------------------------------------------
//  Fast modulo
//------------------------------------------------------------------------------

template<typename T, typename U>
T Delaunay<T, U>::fastMod(const T i, const T c) {

    return i >= c ? i % c : i;
}


//------------------------------------------------------------------------------
//  Explicit template instantiations
//------------------------------------------------------------------------------

template class Delaunay<int64_t, double>;
template class Delaunay<int32_t, double>;
template class Delaunay<int64_t, float>;
template class Delaunay<int32_t, float>;


} // namespace Grain
