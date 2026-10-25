#pragma once

#include <grain/Input/Event.hpp>
#include <grain/Input/Input.hpp>
#include <grain/Platform/Window.hpp>

#include <memory>
#include <string_view>

namespace Grain {

class App {
public:
    static auto instance() -> App&;

    App(const App&) = delete;
    auto operator = (const App&) -> App& = delete;
    App(App&&) = delete;
    auto operator = (App&&) -> App& = delete;

    auto setIcon(const std::string& path) -> bool;
    auto createWindow(std::string_view title, int width, int height) -> Window*;

    [[nodiscard]] auto input() -> Input&;
    [[nodiscard]] auto input() const -> const Input&;

    void run();
    void quit();

private:
    App();
    ~App() = default;

    void processEvent(const Event& event);

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain