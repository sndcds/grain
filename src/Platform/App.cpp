#include <grain/Platform/App.hpp>

#include "Platform.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <iostream>

namespace Grain {

class App::Impl {
public:
    std::unique_ptr<Platform::App> platform_app_;
    std::vector<std::unique_ptr<Window> > windows_;

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


auto App::createWindow(
    std::string_view title,
    int width,
    int height
) -> Window*
{
    if (impl_ == nullptr || impl_->platform_app_ == nullptr) {
        return nullptr;
    }

    const uint64_t window_id = impl_->next_window_id++;

    auto platform_window =
        impl_->platform_app_->createWindow(
            window_id,
            title,
            width,
            height
        );

    if (platform_window == nullptr) {
        return nullptr;
    }

    auto window = Window::create(window_id, std::move(platform_window));
    if (window == nullptr) {
        return nullptr;
    }

    Window* result = window.get();

    impl_->windows_.push_back(std::move(window));

    result->show();

    return result;
}


/* TODO: ???
View* App::addView(std::unique_ptr<View> view) {
    std::cout << "App::addView()" << std::endl;

    if (impl_ == nullptr || impl_->platform_app == nullptr) {
        return nullptr;
    }


    View* result = view.get();
    return result;
}
*/


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

    impl_->running_ = true;
    impl_->input_.beginFrame();
    impl_->platform_app_->run();
    impl_->running_ = false;
}


void App::quit() {
    impl_->running_ = false;

    if (impl_->platform_app_ != nullptr) {
        impl_->platform_app_->quit();
    }
}


void App::processEvent(const Event& event) {
    if (event.type_ == EventType::Quit) {
        quit();
        return;
    }

    impl_->input_.processEvent(event);
}

} // namespace Grain