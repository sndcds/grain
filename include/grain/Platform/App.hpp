#pragma once

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

    void run();

private:
    App();

    ~App();

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain