#pragma once

#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Input/Event.hpp>
#include <grain/UI/Component.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace Grain {

class Window;
class Component;

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

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        auto component = std::make_unique<T>(
            this,
            std::forward<Args>(args)...
            );

        T& result = *component;
        components_.push_back(std::move(component));

        requestRedraw();

        return result;
    }

    void drawComponents(GraphicContext& gc, const Rectd& dirty_rect);

    virtual void draw(GraphicContext& gc, const Rectd& dirty_rect);

    virtual void drawContent(GraphicContext& gc, const Rectd& dirty_rect);

    virtual void requestRedraw();

    void requestRedraw(const Rectd& dirty_rect);

    virtual void handleEvent(const Event& event);

    [[nodiscard]] const Vec2d mousePos() noexcept {
        return queryMousePos();
    }


private:
    class Impl;

    std::unique_ptr<Impl> impl_;

    void attach();

    [[nodiscard]] Platform::View* platformView() noexcept;

    void updateHoveredComponent(Component* component);

    void updateFocusedComponent(Component* component);

    Component* hitTest(const Vec2d& pos) noexcept;

    [[nodiscard]] Vec2d queryMousePos() const noexcept;

    Rectd bounds_{};
    View* parent_ = nullptr;
    Vec2d mouse_pos_{};

    std::vector<std::unique_ptr<Component> > components_;

    // Interaction states
    Component* hovered_component_ = nullptr;
    Component* focused_component_ = nullptr;
    Component* mouse_capture_component_ = nullptr;

    friend class Window;
};

} // namespace Grain