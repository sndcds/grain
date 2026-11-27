#pragma once

#include <grain/Input/Event.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Size.hpp>
#include <grain/String/String.hpp>
#include <grain/Platform/Popup.hpp>

#include <functional>
#include <memory>
#include <string_view>
#include <utility>

namespace Grain {

class View;


namespace Platform {

class View {
public:
    virtual ~View() = default;

    virtual void setEventHandler(EventHandler handler) = 0;

    virtual void requestRedraw() = 0;
    virtual void requestRedraw(const Rectd& dirty_rect) = 0;

    [[nodiscard]] virtual auto nativeView() -> void* = 0;
    [[nodiscard]] virtual auto mousePos() const -> Vec2d = 0;
};


class Window {
public:
    Window(const Window&) = delete;
    auto operator = (const Window&) -> Window& = delete;
    Window(Window&&) = delete;
    auto operator = (Window&&) -> Window& = delete;

    virtual ~Window() = default;

    virtual void show() = 0;
    virtual void requestRedraw() = 0;
    virtual void setRootView(Grain::Platform::View* platformView) = 0;
    virtual void setEventHandler(EventHandler eventHandler) = 0;

    [[nodiscard]]
    virtual auto contentBounds() const -> Rectd = 0;

protected:
    Window() = default;
};


class Popup {
public:
    Popup(const Popup&) = delete;
    auto operator = (const Popup&) -> Popup& = delete;
    Popup(Popup&&) = delete;
    auto operator = (Popup&&) -> Popup& = delete;

    virtual ~Popup() = default;

    virtual void show(
        Window& parent,
        View* content,
        const Rectd& anchor,
        const Sized& size,
        const PopupOptions& options
    ) = 0;

    virtual void hide() = 0;

    virtual void reposition(
        const Rectd& anchor
    ) = 0;

    virtual void resize(
        const Sized& size
    ) = 0;

    [[nodiscard]]
    virtual auto isVisible() const -> bool = 0;

protected:
    Popup() = default;
};


auto createView(Grain::View* view) -> std::unique_ptr<View>;

auto createPopup() -> std::unique_ptr<Popup>;


class App {
public:
    App(const App&) = delete;
    auto operator = (const App&) -> App& = delete;

    App(App&&) = delete;
    auto operator = (App&&) -> App& = delete;

    virtual ~App() = default;

    virtual auto setIcon(const std::string& path) -> bool = 0;

    virtual auto addWindow(
        uint64_t id,
        std::string_view title,
        const Recti& rect
    ) -> std::unique_ptr<Window> = 0;

    void setEventHandler(EventHandler handler) {
        eventHandler_ = std::move(handler);
    }

    virtual void run() = 0;
    virtual void quit() = 0;

protected:
    App() = default;

    void emitEvent(const Event& event) {
        if (eventHandler_) {
            eventHandler_(event);
        }
    }

private:
    EventHandler eventHandler_;
};

auto createApp() -> std::unique_ptr<App>;

} // namespace Platform

} // namespace Grain