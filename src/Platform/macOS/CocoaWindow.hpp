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
        EventHandler eventHandler
        );

    ~CocoaWindow() override;

    void show() override;

    void setRootView(
        Grain::View* view
        ) override;

private:
    WindowId windowId_;

    class Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace Grain::Platform
