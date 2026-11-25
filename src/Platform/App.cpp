#include <grain/Platform/App.hpp>

#include "../../include/grain/Platform/Platform.hpp"

#include <chrono>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>

namespace Grain {

class App::Impl {
public:
    std::unique_ptr<Platform::App> platform_app_;
    std::unordered_map<std::string, std::unique_ptr<Window>> windows_;

    Input input_;
    uint64_t next_window_id = 1;
    bool running_ = false;

    Impl() : platform_app_(Platform::createApp()) {}
};


auto App::instance() -> App& {
    static App app;
    return app;
}


App::App()
    : impl_(std::make_unique<Impl>())
{
    if (impl_->platform_app_ != nullptr) {
        impl_->platform_app_->setEventHandler(
            [this](const Event& event) -> void {
                processEvent(event);
            }
        );
    }
}


auto App::setIcon(const std::string& path) -> bool {
    if (impl_ == nullptr || impl_->platform_app_ == nullptr) {
        return false;
    }

    return impl_->platform_app_->setIcon(path);
}


auto App::addWindow(
    std::string tag,
    std::string_view title,
    const Recti& rect
) -> Window*
{
    static int32_t g_window_id = 0;

    if (tag.empty() || impl_->windows_.contains(tag)) {
        return nullptr;
    }

    const auto id = g_window_id++;

    auto platform_window = impl_->platform_app_->addWindow(
        id,
        title,
        rect
    );

    auto window = Window::create(
        id,
        std::move(platform_window)
    );

    if (!window) {
        return nullptr;
    }

    auto* window_ptr = window.get();

    impl_->windows_.emplace(
        std::move(tag),
        std::move(window)
    );

    return window_ptr;
}


auto App::getWindow(std::string_view tag) -> Window* {

    const auto it = impl_->windows_.find(std::string(tag));

    if (it == impl_->windows_.end()) {
        return nullptr;
    }

    return it->second.get();
}


auto App::getWindow(std::string_view tag) const -> const Window* {

    const auto it = impl_->windows_.find(std::string(tag));

    if (it == impl_->windows_.end()) {
        return nullptr;
    }

    return it->second.get();
}


auto App::getWindowById(uint64_t id) -> Window* {

    for (auto& [tag, window] : impl_->windows_) {
        if (window && window->id() == id) {
            return window.get();
        }
    }

    return nullptr;
}


auto App::input() -> Input& {
    return impl_->input_;
}


auto App::input() const -> const Input& {
    return impl_->input_;
}


void App::run() {
    if (impl_ == nullptr || impl_->platform_app_ == nullptr) {
        return;
    }

    started.emit();

    impl_->running_ = true;
    impl_->input_.beginFrame();

    impl_->platform_app_->run();

    impl_->running_ = false;

}


void App::quit() {
    stopping.emit();

    impl_->running_ = false;

    if (impl_->platform_app_ != nullptr) {
        impl_->platform_app_->quit();
    }

}


void App::processEvent(const Event& event) {

    // Always update global input state first
    impl_->input_.processEvent(event);

    switch (event.type_) {

        case EventType::Quit: {
            quit();
            return;
        }

        case EventType::WindowResize:
        case EventType::MouseMove:
        case EventType::MouseButtonDown:
        case EventType::MouseButtonUp:
        case EventType::MouseWheel:
        case EventType::KeyDown:
        case EventType::KeyUp:
        case EventType::ModifiersChanged:
        case EventType::WindowFocusGained:
        case EventType::WindowFocusLost:
        case EventType::WindowCloseRequested:
        case EventType::WindowWillClose: {
            auto* window = getWindowById(event.window_id_);
            if (window != nullptr) {
                window->handleEvent(event);
            }

            break;
        }

        default:
            break;
        }
    }

} // namespace Grain