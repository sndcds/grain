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


void App::run() {
    if (impl_->platform_app == nullptr) {
        return;
    }

    impl_->platform_app->run();
}

} // namespace Grain