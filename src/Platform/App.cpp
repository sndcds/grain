#include <grain/Platform/App.hpp>

#include "Platform.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace Grain {

class App::Impl {
public:
    std::unique_ptr<Platform::App> platform_app;

    std::vector<std::unique_ptr<Window> > windows;

    Input input;

    bool running = false;

    Impl()
        : platform_app(Platform::createApp()) {
    }
};


App& App::instance() {
    static App app;
    return app;
}


App::App()
    : impl_(std::make_unique<Impl>()) {
    if (impl_->platform_app != nullptr) {
        impl_->platform_app->setEventHandler(
            [this](const Event& event) {
                processEvent(event);
            }
            );
    }
}


App::~App() = default;


Window* App::createWindow(
    std::string_view title,
    int width,
    int height
    ) {
    if (impl_->platform_app == nullptr) {
        return nullptr;
    }

    auto platform_window =
        impl_->platform_app->createWindow(
            title,
            width,
            height
            );

    if (platform_window == nullptr) {
        return nullptr;
    }

    auto window =
        Window::create(
            std::move(platform_window)
            );

    if (window == nullptr) {
        return nullptr;
    }

    Window* result = window.get();

    impl_->windows.push_back(
        std::move(window)
        );

    result->show();

    return result;
}


Input& App::input() noexcept {
    return impl_->input;
}


const Input& App::input() const noexcept {
    return impl_->input;
}


void App::run() {
    if (impl_ == nullptr ||
        impl_->platform_app == nullptr) {
        return;
    }

    impl_->running = true;

    impl_->input.beginFrame();

    impl_->platform_app->run();

    impl_->running = false;
}


void App::quit() noexcept {
    impl_->running = false;

    if (impl_ != nullptr &&
        impl_->platform_app != nullptr) {
        impl_->platform_app->quit();
    }
}


void App::processEvent(const Event& event) {
    if (event.type == EventType::Quit) {
        quit();
        return;
    }

    impl_->input.processEvent(event);
}

} // namespace Grain