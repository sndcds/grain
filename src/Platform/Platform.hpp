#pragma once

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

    virtual void setRootView(
        View* view
        ) = 0;

protected:
    Window() = default;
};


class App {
public:
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

    virtual void run() = 0;
    virtual void quit() = 0;

protected:
    App() = default;
};


std::unique_ptr<App> createApp();

} // namespace Platform
} // namespace Grain