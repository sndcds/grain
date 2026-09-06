#pragma once

#include <memory>
#include <string_view>

namespace Grain {

class App;
class View;

namespace Platform {
class Window;
}

class Window {
public:
    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window&&) = delete;

    ~Window();

    void show();

    void setRootView(
        std::unique_ptr<View> view
        );

private:
    class Impl;

    explicit Window(
        std::unique_ptr<Impl> impl
        );

    static std::unique_ptr<Window> create(
        std::unique_ptr<Platform::Window> platform_window
        );

    std::unique_ptr<Impl> impl_;

    friend class App;
};

} // namespace Grain