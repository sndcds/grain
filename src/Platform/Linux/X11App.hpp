#pragma once

#include "../Platform.hpp"

#include <X11/Xlib.h>

#include <memory>
#include <string_view>

namespace Grain::Platform {

class X11App final : public App {
public:
    X11App();

    ~X11App() override;

    std::unique_ptr<Window> createWindow(
        std::string_view title,
        int width,
        int height
        ) override;

    void run() override;

    [[nodiscard]]
    Display* display() const noexcept {
        return display_;
    }

private:
    Display* display_ = nullptr;
};

} // namespace Grain::Platform