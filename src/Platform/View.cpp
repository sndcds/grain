#include <grain/Platform/View.hpp>

#include "Platform.hpp"
#include <grain/Input/Event.hpp>

#include <memory>
#include <ranges>

namespace Grain {
class View::Impl {
public:
    std::unique_ptr<Platform::View> platformView;
};


View::View()
    : impl_(std::make_unique<Impl>())
{
    std::cout << "View::View()" << '\n';
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
    for (auto & it : std::ranges::reverse_view(components_)) {
        Component* component = it.get();

        if (!component->visible() || !component->enabled()) {
            continue;
        }

        if (component->contains(pos)) {
            return component;
        }
    }

    return nullptr;
}


void View::drawComponents(GraphicContext& gc, const Rectd& dirty_rect) {
    // updateHoveredComponent(hitTest(mousePos()));

    gc.clip(dirty_rect);

    for (auto& component : components_) {
        if (component->visible()) {
            if (dirty_rect.intersects(component->bounds())) {
                component->draw(gc);
            }
        }
    }
}


void View::drawContent(GraphicContext& gc, const Rectd& dirty_rect) {
    (void)gc;
    (void)dirty_rect;
}


void View::draw(GraphicContext& gc, const Rectd& dirty_rect) {
    drawContent(gc, dirty_rect);
    drawComponents(gc, dirty_rect);
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
                std::cout << event.text_ << '\n';
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