#pragma once

#include <grain/Input/Event.hpp>

#include <functional>
#include <memory>
#include <string_view>
#include <utility>

namespace Grain {

class View;

namespace Platform {

class View;

class Window {
public:
    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window&&) = delete;

    virtual ~Window() = default;

    virtual void show() = 0;

    virtual void requestRedraw() = 0;

    virtual void setRootView(Grain::Platform::View* platformView) = 0;

protected:
    Window() = default;
};


class View {
public:
    virtual ~View() = default;

    virtual void requestRedraw() = 0;

    [[nodiscard]]
    virtual void* nativeView() noexcept = 0;
};


std::unique_ptr<View> createView(Grain::View* view);


class App {
public:
    App(const App&) = delete;

    App& operator=(const App&) = delete;

    App(App&&) = delete;

    App& operator=(App&&) = delete;

    virtual ~App() = default;

    virtual std::unique_ptr<Window> createWindow(
        uint64_t id,
        std::string_view title,
        int width,
        int height
        ) = 0;

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

std::unique_ptr<App> createApp();

} // namespace Platform

} // namespace Grain