#pragma once

#include <grain/Graphics/GraphicState.hpp>
#include <grain/Graphics/Color.hpp>
#include <grain/Geometry/Rect.hpp>

#include <vector>

namespace Grain {

class GraphicContext {
public:
    virtual ~GraphicContext() = default;

    GraphicContext(const GraphicContext&) = delete;

    GraphicContext& operator=(const GraphicContext&) = delete;

    GraphicContext(GraphicContext&&) = delete;

    GraphicContext& operator=(GraphicContext&&) = delete;


    //--------------------------------------------------------------------------
    // Stack
    //--------------------------------------------------------------------------

    virtual void save() {
        stateStack_.push_back(state_);
    }

    virtual void restore() {
        if (stateStack_.empty()) {
            return;
        }

        state_ = stateStack_.back();
        stateStack_.pop_back();
    }


    //--------------------------------------------------------------------------
    // State
    //--------------------------------------------------------------------------

    virtual void setFillColor(const Color& color) {
        state_.fillColor = color;
    }

    virtual void setStrokeColor(const Color&  color) {
        state_.strokeColor = color;
    }

    virtual void setLineWidth(double width)  {
        state_.lineWidth = width;
    }

    //--------------------------------------------------------------------------
    // Draw
    //--------------------------------------------------------------------------

    virtual void fillRect(const Rectd& rect) = 0;

    virtual void strokeRect(const Rectd& rect) = 0;


    //--------------------------------------------------------------------------
    // Transform
    //--------------------------------------------------------------------------

    virtual void translate(double x, double y) {
        state_.transform *= Mat3<double>::translation(x, y);
    }

    virtual void scale(double x, double y) {
        state_.transform *= Mat3<double>::scaling(x, y);
    }

    virtual void rotate(double radians) {
        state_.transform *= Mat3<double>::rotation(radians);
    }

    virtual void rotateDegrees(double degrees) {
        state_.transform *= Mat3<double>::rotationDegrees(degrees);
    }


protected:
    GraphicContext() = default;

    GraphicState state_;
    std::vector<GraphicState> stateStack_;
};

} // namespace Grain