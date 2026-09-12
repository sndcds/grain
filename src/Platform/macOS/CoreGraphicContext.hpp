#pragma once

#include <CoreGraphics/CoreGraphics.h>

#include <grain/Graphics/GraphicContext.hpp>

namespace Grain {

class CoreGraphicContext final : public GraphicContext {
public:
    explicit CoreGraphicContext(CGContextRef context);

    ~CoreGraphicContext() override = default;

    void save() override;

    void restore() override;

    void setFillColor(const Color& color) override;

    void setStrokeColor(const Color& color) override;

    void setLineWidth(double width) override;

    void fillRect(const Rectd& rect) override;

    void strokeRect(const Rectd& rect) override;

    void translate(double x, double y) override;

    void scale(double x, double y) override;

    void rotate(double radians) override;

    void rotateDegrees(double degrees) override;

private:
    CGContextRef context_ = nullptr;
};

} // namespace Grain