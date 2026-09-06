#pragma once

#include <grain/Core/ObjectBase.hpp>
#include <grain/Graphics/Canvas.hpp>

namespace Grain {

class Canvas;

class View : public ObjectBase {
public:
    using Coordinate = double;

    using Rect = Canvas::Rect;

    View() = default;

    ~View() override = default;

    View(const View&) = delete;

    View& operator=(const View&) = delete;

    View(View&&) = delete;

    View& operator=(View&&) = delete;

    [[nodiscard]]
    const Rect& bounds() const noexcept {
        return bounds_;
    }

    void setBounds(const Rect& bounds) noexcept {
        bounds_ = bounds;
    }

    [[nodiscard]]
    View* parent() const noexcept {
        return parent_;
    }

    virtual void draw(Canvas& canvas);

private:
    Rect bounds_{};
    View* parent_ = nullptr;
};

} // namespace Grain