#pragma once

#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Input/Event.hpp>

#include <memory>

namespace Grain {

class Window;

namespace Platform {
class View;
}

class View {
public:
    View();

    View(const View&) = delete;
    View& operator=(const View&) = delete;

    View(View&&) = delete;
    View& operator=(View&&) = delete;

    virtual ~View();

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
    virtual void handleEvent(const Event& event);
    virtual void requestRedraw();

private:
    class Impl;

    std::unique_ptr<Impl> impl_;

    void attach();

    [[nodiscard]]
    Platform::View* platformView() noexcept;

    Rectd bounds_{};
    View* parent_ = nullptr;

    friend class Window;
};

} // namespace Grain