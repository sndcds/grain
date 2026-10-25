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

    virtual ~View();

    auto operator = (const View&) -> View& = delete;
    View(View&&) = delete;
    auto operator = (View&&) -> View& = delete;


    [[nodiscard]] auto bounds() const -> const Rectd& { return bounds_; }
    void setBounds(const Rectd& bounds) { bounds_ = bounds; }

    [[nodiscard]] auto parent() const -> View* { return parent_; }

    template <typename T, typename... Args>
    auto addComponent(Args&&... args) -> T& {
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
    virtual void drawContent(GraphicContext& gc, const Rectd& dirty_rect);
    virtual void draw(GraphicContext& gc, const Rectd& dirty_rect);

    virtual void requestRedraw();
    void requestRedraw(const Rectd& dirty_rect);

    virtual void handleMouseEvent(const Event& event);
    virtual void handleKeyEvent(const Event& event);
    virtual void handleEvent(const Event& event);

    [[nodiscard]] auto mousePos() -> Vec2d { return queryMousePos(); }

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    void attach();

    [[nodiscard]] auto platformView() -> Platform::View*;

    void updateHoveredComponent(Component* component);
    void updateFocusedComponent(Component* component);
    auto hitTest(const Vec2d& pos) -> Component*;
    [[nodiscard]] auto queryMousePos() const -> Vec2d;

    Rectd bounds_;
    View* parent_ = nullptr;
    Vec2d mouse_pos_;

    std::vector<std::unique_ptr<Component> > components_;

    // Interaction states
    Component* hovered_component_ = nullptr;
    Component* focused_component_ = nullptr;
    Component* mouse_capture_component_ = nullptr;

    friend class Window;
};

} // namespace Grain