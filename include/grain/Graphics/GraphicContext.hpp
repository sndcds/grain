#pragma once

#include <iostream>
#include <grain/Graphics/GraphicState.hpp>
#include <grain/Graphics/Color.hpp>
#include <grain/Graphics/Font.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Graphics/GraphicPath.hpp>

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

    virtual void save() = 0;

    virtual void restore() = 0;
/*
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
*/

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
    // Path
    //--------------------------------------------------------------------------

    virtual void beginPath() noexcept = 0;

    virtual void moveTo(const Vec2d& point) noexcept = 0;

    virtual void lineTo(const Vec2d& point) noexcept = 0;

    virtual void curveTo(const Vec2d& control1, const Vec2d& control2, const Vec2d& point) noexcept = 0;

    virtual void closePath() noexcept = 0;

    void addPath(const GraphicPath& path) noexcept {
        if (path.pointCount() > 1) {
            const GraphicPathPoint* firstPoint = nullptr;
            const GraphicPathPoint* prevPoint = nullptr;

            for (int32_t i = 0; i < path.pointCount(); i++) {
                auto point = path.pointPtrAtIndex(i);

                if (i == 0) {
                    moveTo(point->anchor);
                    firstPoint = point;
                }
                else {
                    if (prevPoint->rightFlag && point->leftFlag) {
                        curveTo(prevPoint->right, point->left, point->anchor);
                    }
                    else if (prevPoint->rightFlag) {
                        curveTo(prevPoint->right, point->anchor, point->anchor);
                    }
                    else if (point->leftFlag) {
                        curveTo(prevPoint->anchor, point->left, point->anchor);
                    }
                    else {
                        lineTo(point->anchor);
                    }
                }

                prevPoint = point;
            }

            if (path.isClosed()) {
                if (firstPoint) {
                    if (prevPoint->rightFlag && firstPoint->leftFlag) {
                        curveTo(prevPoint->right, firstPoint->left, firstPoint->anchor);
                    }
                    else if (prevPoint->rightFlag) {
                        curveTo(prevPoint->right, firstPoint->anchor, firstPoint->anchor);
                    }
                    else if (firstPoint->leftFlag) {
                        curveTo(prevPoint->anchor, firstPoint->left, firstPoint->anchor);
                    }
                    else {
                        lineTo(firstPoint->anchor);
                    }
                }

                closePath();
            }
        }
    }


    //--------------------------------------------------------------------------
    // Draw
    //--------------------------------------------------------------------------

    virtual void fillRect(const Rectd& rect) = 0;

    virtual void strokeRect(const Rectd& rect) = 0;

    virtual void fillPath(const GraphicPath& path) = 0;

    virtual void strokePath(const GraphicPath& path) = 0;

    virtual void drawText(const String& text, const Vec2d& pos, const Font* font, const Color& color) = 0;


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