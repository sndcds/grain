#pragma once

#include "../Platform.hpp"

#include <string_view>
#include <functional>

namespace Grain::Platform {

class CocoaWindow final : public Window {
public:
    CocoaWindow(
        uint64_t id,
        std::string_view title,
        int width,
        int height,
        EventHandler eventHandler
        );

    ~CocoaWindow() override;

    void show() override;

    void requestRedraw() override;

    void setRootView(View* platformView) override;

private:
    uint64_t windowId_;

    class Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace Grain::Platform
