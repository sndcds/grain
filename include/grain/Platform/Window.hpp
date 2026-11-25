#pragma once

#include <grain/Input/Event.hpp>
#include <grain/UI/LayoutEngine.hpp>
#include <grain/Events/Emitter.hpp>

#include <sigslot/signal.hpp>

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


//-----------------------------------------------------------------------------
//  Grain::Window
//
//  The portable public/window abstraction.
//------------------------------------------------------------------------------

class Window : public sigslot::observer_st {
public:
    Window(const Window&) = delete;
    auto operator = (const Window&) -> Window& = delete;
    Window(Window&&) = delete;
    auto operator = (Window&&) -> Window& = delete;

    virtual ~Window();

    [[nodiscard]] auto id() const -> uint64_t;

    [[nodiscard]] auto rootView() -> View&;
    [[nodiscard]] auto rootView() const -> const View&;

    [[nodiscard]] auto contentBounds() const -> Rectd;

    void show();
    void requestRedraw();

    void setRootView(std::unique_ptr<View> view);
    virtual void handleEvent(const Event& event);

    void updateLayout();

    Grain::Events::Emitter<> closing;
    Grain::Events::Emitter<int32_t, int32_t> resized;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    LayoutEngine layout_engine_;

    explicit Window(std::unique_ptr<Impl> impl);

    static auto create(
        uint64_t id,
        std::unique_ptr<Platform::Window> platformWindow
        ) -> std::unique_ptr<Window>;


    friend class App;
};

} // namespace Grain
