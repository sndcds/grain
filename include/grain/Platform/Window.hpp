#pragma once

#include <memory>
#include <string_view>

namespace Grain {

class App;
class View;

class Window {
public:
    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window&&) = delete;

    ~Window();

    void show();

    void setRootView(std::unique_ptr<View> view);

private:
    class Impl;

    explicit Window(
        std::string_view title,
        int width,
        int height
        );

    std::unique_ptr<Impl> impl_;

    friend class App;
};

} // namespace Grain