#include <grain/UI/View.hpp>
#include <grain/UI/UIElement.hpp>

#include <grain/Platform/Platform.hpp>
#include <grain/Input/Event.hpp>

#include <memory>
#include <ranges>
#include <iostream>

namespace Grain {
class View::Impl {
public:
    std::unique_ptr<Platform::View> platformView;

    Impl();
    ~Impl();
};

View::Impl::Impl() = default;
View::Impl::~Impl() = default;


View::View()
    : impl_(std::make_unique<Impl>())
{
}


View::~View() = default;


void View::attach() {
    if (impl_->platformView != nullptr) {
        return;
    }

    impl_->platformView =
        Platform::createView(this);
}


auto View::platformView() -> Platform::View* {
    if (impl_ == nullptr) {
        return nullptr;
    }

    return impl_->platformView.get();
}


void View::updateHoveredComponent(Component* component) {
    if (hovered_component_ == component) {
        return;
    }

    if (hovered_component_ != nullptr) {
        hovered_component_->setHovered(false);
    }

    hovered_component_ = component;

    if (hovered_component_ != nullptr) {
        hovered_component_->setHovered(true);
    }
}


void View::updateFocusedComponent(Component* component) {
    if (focused_component_ == component) {
        return;
    }

    if (focused_component_ != nullptr) {
        focused_component_->setFocused(false);
    }

    focused_component_ = component;

    if (focused_component_ != nullptr) {
        focused_component_->setFocused(true);
    }
}


auto View::hitTest(const Vec2d& pos) -> Component* {

    for (auto& it : std::ranges::reverse_view(children_)) {

        UIElement* child = it.get();

        switch (child->type()) {

        case UIElementType::Component: {
            auto* component = static_cast<Component*>(child);

            if (!component->visible() || !component->enabled()) {
                continue;
            }

            if (component->contains(pos)) {
                return component;
            }

            break;
        }

        case UIElementType::View: {
            auto* view = static_cast<View*>(child);

            if (auto* component = view->hitTest(pos)) {
                return component;
            }

            break;
        }
        }
    }

    return nullptr;
}


void View::drawComponents(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    // updateHoveredComponent(hitTest(mousePos()));

    gc.clip(dirty_rect);

    std::cout << "View::drawComponents: " << dirty_rect << std::endl;

    for (auto& child : children_) {
        if (child->type() == UIElementType::Component) {

            std::cout << "Component" << std::endl;

            auto* component = static_cast<Component*>(child.get());
            if (dirty_rect.intersects(component->bounds())) {
                std::cout << "Component: " << component->bounds() << std::endl;
                component->draw(gc);
            }
        }
    }
}


void View::drawContent(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    (void)gc;
    (void)dirty_rect;
}


void View::draw(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    auto rect = dirty_rect;
    if (rect.y < 0) {
        rect.height += rect.y;
        rect.y = 0;
    }
    gc.setFillColor({1, 0, 0, 1});
    gc.fillRect(rect);

    gc.setFillColor({1, 1, 0, 1});
    gc.fillRect({10, 10, 200, 200});

    drawContent(gc, dirty_rect);
    drawComponents(gc, dirty_rect);
}


void View::doSizing() {
    std::cout << "View::doSizing()" << std::endl;
}


void View::handleMouseEvent(const Event& event) {
    switch (event.type_) {
        case EventType::MouseMove: {
            if (mouse_capture_component_ != nullptr) {
                mouse_capture_component_->handleEvent(event);
            } else {
                updateHoveredComponent(hitTest(event.mouse_pos_));
            }
            break;
        }

        case EventType::MouseButtonDown: {
            Component* component = hitTest(event.mouse_pos_);

            if (component != nullptr && component->handleEvent(event)) {
                mouse_capture_component_ = component;
            }
            break;
        }

        case EventType::MouseButtonUp: {
            if (mouse_capture_component_ != nullptr) {
                Component* component = mouse_capture_component_;
                mouse_capture_component_ = nullptr;

                component->handleEvent(event);
            } else {
                Component* component = hitTest(event.mouse_pos_);

                if (component != nullptr) {
                    component->handleEvent(event);
                }
            }
            break;
        }

        default:
            break;
    }
}


void View::handleKeyEvent(const Event& event) {
    switch (event.type_) {
        case EventType::KeyDown: {
            if (focused_component_ != nullptr) {
            }
            break;
        }

        default:
            break;
    }
}


void View::handleEvent(const Event& event) {
    switch (event.type_) {
        default: {
            if (focused_component_ != nullptr &&
                focused_component_->visible() &&
                focused_component_->enabled()) {
                focused_component_->handleEvent(event);
                }
            break;
        }
    }
}


void View::requestRedraw() {
    if (impl_->platformView == nullptr) {
        return;
    }

    impl_->platformView->requestRedraw();
}


void View::requestRedraw(const Rectd& dirty_rect) {
    if (impl_->platformView == nullptr) {
        return;
    }

    impl_->platformView->requestRedraw(dirty_rect);
}


auto View::queryMousePos() const -> Vec2d {
    if (impl_ == nullptr || impl_->platformView == nullptr) {
        return {};
    }

    return impl_->platformView->mousePos();
}


} // namespace Grain