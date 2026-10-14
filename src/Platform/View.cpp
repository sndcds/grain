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


Platform::View* View::platformView() noexcept {
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


Component* View::hitTest(const Vec2d& pos) noexcept {
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


void View::drawComponents(GraphicContext& gc) {
    updateHoveredComponent(hitTest(mousePos()));

    for (auto& component : components_) {
        if (component->visible()) {
            component->draw(gc);
        }
    }
}


void View::drawContent(GraphicContext& gc) {
    (void)gc;
}


void View::draw(GraphicContext& gc) {
    drawContent(gc);
    drawComponents(gc);
}


void View::handleEvent(const Event& event) {
    switch (event.type_) {
        case EventType::MouseMove: {
            updateHoveredComponent(hitTest(event.mouse_pos_));
            break;
        }

        case EventType::MouseButtonDown:
        case EventType::MouseButtonUp: {
            Component* component = hitTest(event.mouse_pos_);

            if (component != nullptr) {
                component->handleEvent(event);
            }
            break;
        }

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


Vec2d View::queryMousePos() const noexcept {
    if (impl_ == nullptr || impl_->platformView == nullptr) {
        return {};
    }

    return impl_->platformView->mousePos();
}


} // namespace Grain