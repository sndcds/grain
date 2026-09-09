#pragma once

#include <memory>
#include <string_view>

namespace Grain {

class App;
class View;

namespace Platform {
class Window;
}

using WindowId = std::uint64_t;

class Window {
public:
    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window&&) = delete;

    ~Window();

    WindowId id() const noexcept {
        return id_;
    }

    void show();

    void setRootView(
        std::unique_ptr<View> view
        );

private:
    WindowId id_ = 0;

    class Impl;

    explicit Window(
        std::unique_ptr<Impl> impl
        );

    static std::unique_ptr<Window> create(
        WindowId id,
        std::unique_ptr<Platform::Window> platform_window
    );

    std::unique_ptr<Impl> impl_;

    friend class App;
};

} // namespace Grain