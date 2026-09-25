#include <grain/Platform/View.hpp>

#include "Platform.hpp"

#include <memory>

namespace Grain {
class View::Impl {
public:
    std::unique_ptr<Platform::View> platformView;
};


View::View()
    : impl_(std::make_unique<Impl>())
{
    std::cout << "View::View()" << std::endl;
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


void View::draw(GraphicContext& gc) {
    drawContent(gc);
    drawComponents(gc);
}


void View::drawContent(GraphicContext& gc) {
    (void)gc;
}


void View::handleEvent(const Event& event) {

    for (auto it = components_.rbegin();
        it != components_.rend();
        ++it) {
        if ((*it)->visible() &&
            (*it)->handleEvent(event)) {
        }
    }

    (void)event;
}


void View::requestRedraw() {
    if (impl_->platformView == nullptr) {
        return;
    }

    impl_->platformView->requestRedraw();
}

} // namespace Grain