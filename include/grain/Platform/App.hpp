#pragma once

#include <grain/Input/Event.hpp>
#include <grain/Input/Input.hpp>
#include <grain/Platform/Window.hpp>

#include <memory>
#include <string_view>

namespace Grain {

class App {
public:
    static App& instance();

    App(const App&) = delete;

    App& operator=(const App&) = delete;

    App(App&&) = delete;

    App& operator=(App&&) = delete;

    Window* createWindow(
        std::string_view title,
        int width,
        int height
        );

    Input& input() noexcept;

    const Input& input() const noexcept;

    void run();

    void quit() noexcept;

private:
    App();

    ~App();

    void processEvent(const Event& event);

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain