#pragma once

#include "../Platform.hpp"

#include <string_view>
#include <functional>

namespace Grain::Platform {

class CocoaWindow final : public Window {
public:
    CocoaWindow(
        WindowId id,
        std::string_view title,
        int width,
        int height,
        EventHandler event_handler
        );

    ~CocoaWindow() override;

    void show() override;

    void setRootView(
        Grain::View* view
        ) override;

private:
    WindowId window_id_;

    class Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace Grain::Platform
