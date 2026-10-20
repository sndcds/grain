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
    auto operator=(const Window&) -> Window& = delete;
    Window(Window&&) = delete;
    auto operator=(Window&&) -> Window& = delete;

    virtual ~Window();

    [[nodiscard]] auto id() const -> uint64_t;

    void show();
    void requestRedraw();

    void setRootView(std::unique_ptr<View> view);
    virtual void handleEvent(const Event& event);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    explicit Window(std::unique_ptr<Impl> impl);

    static auto create(
        uint64_t id,
        std::unique_ptr<Platform::Window> platformWindow
        ) -> std::unique_ptr<Window>;


    friend class App;
};

} // namespace Grain
