#include <grain/Graphics/GraphicCompoundPath.hpp>
#include <grain/Graphics/GraphicPathPoint.hpp>
#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Text/Font.hpp>
#include <grain/String/String.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>


namespace Grain {


//------------------------------------------------------------------------------
//  Information
//------------------------------------------------------------------------------

auto GraphicCompoundPath::pathPtrAtIndex(int32_t index) -> GraphicPath* {

    if (index < 0 || index >= pathCount()) {
        return nullptr;
    }

    return &paths_[static_cast<std::size_t>(index)];
}


auto GraphicCompoundPath::pathPtrAtIndex(int32_t index) const -> const GraphicPath* {

    if (index < 0 || index >= pathCount()) {
        return nullptr;
    }

    return &paths_[static_cast<std::size_t>(index)];
}


auto GraphicCompoundPath::lastPathPtr() -> GraphicPath* {

    if (paths_.empty()) {
        return nullptr;
    }

    return &paths_.back();
}


auto GraphicCompoundPath::lastPathPtr() const -> const GraphicPath* {

    if (paths_.empty()) {
        return nullptr;
    }

    return &paths_.back();
}


//------------------------------------------------------------------------------
//  Geometry
//------------------------------------------------------------------------------

auto GraphicCompoundPath::bounds() const -> Rectd {

    Rectd result;
    bounds(result);
    return result;
}


auto GraphicCompoundPath::bounds(Rectd& out_bounds) const -> bool {

    bool has_bounds = false;

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double max_y = std::numeric_limits<double>::lowest();

    for (const auto& path : paths_) {

        Rectd pathBounds;

        if (!path.bounds(pathBounds)) {
            continue;
        }

        min_x = std::min(min_x, pathBounds.x);
        min_y = std::min(min_y, pathBounds.y);

        max_x = std::max(max_x, pathBounds.x + pathBounds.width);
        max_y = std::max(max_y, pathBounds.y + pathBounds.height);

        has_bounds = true;
    }

    if (!has_bounds) {
        out_bounds = Rectd{};
        return false;
    }

    out_bounds.x = min_x;
    out_bounds.y = min_y;
    out_bounds.width = max_x - min_x;
    out_bounds.height = max_y - min_y;

    return true;
}


auto GraphicCompoundPath::polygonCentroid(Vec2d& out_centroid) const -> double {

    double total_area = 0.0;
    Vec2d weighted_centroid;

    for (const auto& path : paths_) {

        Vec2d path_centroid;
        const double area = path.polygonCentroid(path_centroid);

        if (area != 0.0) {
            weighted_centroid += path_centroid * area;
            total_area += area;
        }
    }

    if (std::abs(total_area) > std::numeric_limits<double>::epsilon()) {
        out_centroid = weighted_centroid * (1.0 / total_area);
    }

    return total_area;
}


//------------------------------------------------------------------------------
//  Modification
//------------------------------------------------------------------------------

void GraphicCompoundPath::clear() {

    paths_.clear();
    offset_.zero();
    must_add_path_ = true;
}


//------------------------------------------------------------------------------
//  Path construction
//------------------------------------------------------------------------------

void GraphicCompoundPath::addEmptyPath(int32_t point_capacity) {

    paths_.emplace_back(point_capacity);
    must_add_path_ = false;
}


//------------------------------------------------------------------------------
//  Build from text
//------------------------------------------------------------------------------

auto GraphicCompoundPath::buildFromText(
    const Font& font,
    const char* text
) -> Rectd {

    clear();

    if (text == nullptr || text[0] == '\0') {
        return {};
    }

    const FontPath font_path = font.buildPath(text);

    for (const auto& element : font_path.elements) {
        addFontPathElement(element);
    }

    finish();

    return font_path.bounds;
}

auto GraphicCompoundPath::buildFromText(
    const Font& font,
    const String& text
) -> Rectd {
    return buildFromText(font, text.utf8());
}


//------------------------------------------------------------------------------
//  Transformation
//------------------------------------------------------------------------------

void GraphicCompoundPath::finish() {

    offset_.zero();
    must_add_path_ = true;
}


void GraphicCompoundPath::projectToQuadrilateral(
    const Quadrilateral& quadrilateral,
    const Mat3d* matrix
) {

    for (auto& path : paths_) {
        path.projectToQuadrilateral(quadrilateral, matrix);
    }
}


void GraphicCompoundPath::addFontPathElement(
    const FontPathElement& element
) {
    switch (element.type) {
        case FontPathElement::Type::MoveTo: {
            addEmptyPath();

            auto* path = lastPathPtr();

            if (path) {
                path->addPoint(
                    element.points[0].x,
                    -element.points[0].y
                );
            }
            break;
        }

        case FontPathElement::Type::LineTo: {
            auto* path = lastPathPtr();

            if (path && !must_add_path_) {
                path->addPoint(
                    element.points[0].x,
                    -element.points[0].y
                );
            }
            break;
        }

        case FontPathElement::Type::QuadraticTo: {
            auto* path = lastPathPtr();

            if (!path || must_add_path_) {
                break;
            }

            auto* last_point = static_cast<GraphicPathPoint*>(
                path->lastPointPtr()
            );

            if (!last_point) {
                break;
            }

            const double cx = element.points[0].x;
            const double cy = -element.points[0].y;

            const double x = element.points[1].x;
            const double y = -element.points[1].y;

            const double c1x =
                last_point->anchor.x +
                (2.0 / 3.0) * (cx - last_point->anchor.x);

            const double c1y =
                last_point->anchor.y +
                (2.0 / 3.0) * (cy - last_point->anchor.y);

            const double c2x = x + (2.0 / 3.0) * (cx - x);
            const double c2y = y + (2.0 / 3.0) * (cy - y);

            last_point->right = Vec2d(c1x, c1y);
            last_point->right_flag = true;

            path->addPointLeft(x, y, c2x, c2y);

            break;
        }

        case FontPathElement::Type::CubicTo: {
            auto* path = lastPathPtr();

            if (!path || must_add_path_) {
                break;
            }

            auto* last_point = static_cast<GraphicPathPoint*>(
                path->lastPointPtr()
            );

            if (!last_point) {
                break;
            }

            last_point->right = Vec2d(
                element.points[0].x,
                -element.points[0].y
            );
            last_point->right_flag = true;

            path->addPointLeft(
                element.points[2].x,
                -element.points[2].y,
                element.points[1].x,
                -element.points[1].y
            );

            break;
        }

        case FontPathElement::Type::Close: {
            auto* path = lastPathPtr();

            if (path && !must_add_path_) {
                path->setClosed(true);
                must_add_path_ = true;
            }
            break;
        }
    }
}

} // namespace Grain
