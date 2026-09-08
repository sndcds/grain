#pragma once

#include <grain/Input/Event.hpp>

#include <functional>
#include <memory>
#include <string_view>

namespace Grain {

class View;

namespace Platform {

class Window {
public:
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    virtual ~Window() = default;

    virtual void show() = 0;
    virtual void setRootView(View* view) = 0;

protected:
    Window() = default;
};

class App {
public:
    using EventHandler = std::function<void(const Event&)>;

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    App(App&&) = delete;
    App& operator=(App&&) = delete;

    virtual ~App() = default;

    virtual std::unique_ptr<Window> createWindow(
        std::string_view title,
        int width,
        int height
    ) = 0;

    void setEventHandler(EventHandler handler) {
        event_handler_ = std::move(handler);
    }

    virtual void run() = 0;

    virtual void quit() {}

protected:
    App() = default;

    void emitEvent(const Event& event) {
        if (event_handler_) {
            event_handler_(event);
        }
    }

private:
    EventHandler event_handler_;
};

std::unique_ptr<App> createApp();

} // namespace Platform
} // namespace Grain