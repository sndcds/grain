#pragma once

#include <CoreGraphics/CoreGraphics.h>

#include <grain/Graphics/GraphicsContext.hpp>

namespace Grain {

class CoreGraphicsContext final : public GraphicsContext {
public:
    explicit CoreGraphicsContext(CGContextRef context);

    ~CoreGraphicsContext() override = default;

    void save() override;

    void restore() override;

    void setFillColor(Color color) override;

    void setStrokeColor(Color color) override;

    void setLineWidth(double width) override;

    void fillRect(const Rect& rect) override;

    void strokeRect(const Rect& rect) override;

    void translate(double x, double y) override;

    void scale(double x, double y) override;

    void rotate(double radians) override;

private:
    CGContextRef context_ = nullptr;
};

} // namespace Grain