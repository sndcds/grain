#include <grain/Platform/App.hpp>

#include "Platform.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <iostream>

namespace Grain {

class App::Impl {
public:
    std::unique_ptr<Platform::App> platformApp;
    std::vector<std::unique_ptr<Window> > windows;

    Input input;

    uint64_t nextWindowId = 1;

    bool running = false;

    Impl()
        : platformApp(Platform::createApp()) {
        std::cout << "App::Impl()" << std::endl;
    }
};


App& App::instance() {
    static App app;
    std::cout << "App::instance()" << std::endl;
    return app;
}


App::App()
    : impl_(std::make_unique<Impl>())
{
    std::cout << "App::App()" << std::endl;
    if (impl_->platformApp != nullptr) {
        impl_->platformApp->setEventHandler(
            [this](const Event& event) {
                processEvent(event);
            }
        );
    }
}


Window* App::createWindow(
    std::string_view title,
    int width,
    int height
    )
{
    std::cout << "App::createWindow()" << std::endl;
    if (impl_ == nullptr || impl_->platformApp == nullptr) {
        return nullptr;
    }

    const uint64_t windowId = impl_->nextWindowId++;

    auto platformWindow =
        impl_->platformApp->createWindow(
            windowId,
            title,
            width,
            height
        );

    if (platformWindow == nullptr) {
        return nullptr;
    }

    auto window =
        Window::create(
            windowId,
            std::move(platformWindow)
        );

    if (window == nullptr) {
        return nullptr;
    }

    Window* result = window.get();

    impl_->windows.push_back(std::move(window));

    result->show();

    return result;
}


/*
View* App::addView(std::unique_ptr<View> view) {
    std::cout << "App::addView()" << std::endl;

    if (impl_ == nullptr || impl_->platformApp == nullptr) {
        return nullptr;
    }


    View* result = view.get();
    return result;
}
*/


Input& App::input() noexcept {
    return impl_->input;
}


const Input& App::input() const noexcept {
    return impl_->input;
}


void App::run() {
    std::cout << "App::run()" << std::endl;

    if (impl_ == nullptr || impl_->platformApp == nullptr) {
        return;
    }

    impl_->running = true;

    impl_->input.beginFrame();

    impl_->platformApp->run();

    impl_->running = false;
}


void App::quit() noexcept {
    std::cout << "App::quit()" << std::endl;
    impl_->running = false;

    if (impl_->platformApp != nullptr) {
        impl_->platformApp->quit();
    }
}


void App::processEvent(const Event& event) {
    std::cout << "App::processEvent()" << std::endl;
    std::cout
        << "  type: " << static_cast<int>(event.type)
        << ", windowId: " << static_cast<int>(event.windowId)
        << ", mouseButton: " << static_cast<int>(event.mouseButton)
        << ", " << event.mouseX
        << ", " << event.mouseY
        << '\n';

    if (event.type == EventType::Quit) {
        quit();
        return;
    }

    impl_->input.processEvent(event);
}

} // namespace Grain