#pragma once

#include <grain/Graphics/Color.hpp>
#include <grain/Geometry/Rect.hpp>

namespace Grain {

class GraphicContext {
public:
    virtual ~GraphicContext() = default;

    GraphicContext(const GraphicContext&) = delete;

    GraphicContext& operator=(const GraphicContext&) = delete;

    GraphicContext(GraphicContext&&) = delete;

    GraphicContext& operator=(GraphicContext&&) = delete;

    virtual void save() = 0;

    virtual void restore() = 0;

    virtual void setFillColor(Color color) = 0;

    virtual void setStrokeColor(Color color) = 0;

    virtual void setLineWidth(double width) = 0;

    virtual void fillRect(const Rectd& rect) = 0;

    virtual void strokeRect(const Rectd& rect) = 0;

    virtual void translate(double x, double y) = 0;

    virtual void scale(double x, double y) = 0;

    virtual void rotate(double radians) = 0;

protected:
    GraphicContext() = default;
};

} // namespace Grain