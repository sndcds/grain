#pragma once

#include <grain/Core/ObjectBase.hpp>
#include <grain/Graphics/GraphicContext.hpp>

namespace Grain {

class View : public ObjectBase {
public:
    using Coordinate = double;

    View() = default;

    ~View() override = default;

    View(const View&) = delete;

    View& operator=(const View&) = delete;

    View(View&&) = delete;

    View& operator=(View&&) = delete;

    [[nodiscard]]
    const Rectd& bounds() const noexcept {
        return bounds_;
    }

    void setBounds(const Rectd& bounds) noexcept {
        bounds_ = bounds;
    }

    [[nodiscard]]
    View* parent() const noexcept {
        return parent_;
    }

    virtual void draw(GraphicContext& context);

private:
    Rectd bounds_{};
    View* parent_ = nullptr;
};

} // namespace Grain