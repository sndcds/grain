
#pragma once

#include <grain/Graphics/GraphicPath.hpp>
#include <grain/Graphics/FontPath.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Math/Vec2.hpp>
#include <grain/Container/List.hpp>

#include <cstdint>


namespace Grain {

class Font;

class GraphicCompoundPath {
public:

    //--------------------------------------------------------------------------
    //  Construction / destruction
    //--------------------------------------------------------------------------

    GraphicCompoundPath() = default;
    ~GraphicCompoundPath() = default;

    GraphicCompoundPath(const GraphicCompoundPath&) = default;
    auto operator = (const GraphicCompoundPath&) -> GraphicCompoundPath& = default;

    GraphicCompoundPath(GraphicCompoundPath&&) noexcept = default;
    auto operator = (GraphicCompoundPath&&) noexcept -> GraphicCompoundPath& = default;


    //--------------------------------------------------------------------------
    //  Information
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto hasPaths() const -> bool {
        return !paths_.empty();
    }

    [[nodiscard]]
    auto pathCount() const -> int32_t {
        return static_cast<int32_t>(paths_.size());
    }

    [[nodiscard]]
    auto lastPathIndex() const -> int32_t {
        return pathCount() - 1;
    }

    [[nodiscard]]
    auto pathPtrAtIndex(int32_t index) -> GraphicPath*;

    [[nodiscard]]
    auto pathPtrAtIndex(int32_t index) const -> const GraphicPath*;

    [[nodiscard]]
    auto lastPathPtr() -> GraphicPath*;

    [[nodiscard]]
    auto lastPathPtr() const -> const GraphicPath*;


    //--------------------------------------------------------------------------
    //  Geometry
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto bounds() const -> Rectd;

    [[nodiscard]]
    auto bounds(Rectd& out_bounds) const -> bool;

    [[nodiscard]]
    auto polygonCentroid(Vec2d& out_centroid) const -> double;


    //--------------------------------------------------------------------------
    //  Modification
    //--------------------------------------------------------------------------

    void clear();

    void clearOffset() {
        offset_.zero();
    }

    void setOffset(double x, double y) {
        offset_.x = x;
        offset_.y = y;
    }


    //--------------------------------------------------------------------------
    //  Path construction
    //--------------------------------------------------------------------------

    void addEmptyPath(int32_t point_capacity = 32);



    //--------------------------------------------------------------------------
    //  Transformation
    //--------------------------------------------------------------------------

    void finish();

    void projectToQuadrilateral(
        const Quadrilateral& quadrilateral,
        const Mat3d* matrix = nullptr
    );


    //--------------------------------------------------------------------------
    //  Build from text
    //--------------------------------------------------------------------------

    [[nodiscard]] auto buildFromText(
        const Font& font,
        const char* text
    ) -> Rectd;

    [[nodiscard]] auto buildFromText(
        const Font& font,
        const String& text
    ) -> Rectd;


private:

    List<GraphicPath> paths_;

    Vec2d offset_;

    bool must_add_path_ = true;

    void addFontPathElement(const FontPathElement& element);
};


} // namespace Grain
