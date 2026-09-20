#pragma once

#include <grain/Input/Event.hpp>

#include <memory>


namespace Grain {

//------------------------------------------------------------------------------
//  Forward declarations.
//------------------------------------------------------------------------------

class App;
class View;

namespace Platform {
class Window;
}


//------------------------------------------------------------------------------
//  Grain::Window
//
//  The portable public/window abstraction.
//------------------------------------------------------------------------------

class Window {
public:
    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window&&) = delete;

    ~Window();

    uint64_t id() const noexcept;

    void show();

    void requestRedraw();

    void setRootView(std::unique_ptr<View> view);

private:
    class Impl;

    explicit Window(std::unique_ptr<Impl> impl);

    static std::unique_ptr<Window> create(
        uint64_t id,
        std::unique_ptr<Platform::Window> platformWindow
        );

    std::unique_ptr<Impl> impl_;

    friend class App;
};

} // namespace Grain
