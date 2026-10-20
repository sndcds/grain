#pragma once

#include "../Platform.hpp"

#include <string_view>

namespace Grain::Platform {

class CocoaWindow final : public Window {
public:
    CocoaWindow(
        uint64_t id,
        std::string_view title,
        int width,
        int height,
        EventHandler event_handler
    );

    ~CocoaWindow() override;

    void show() override;

    void requestRedraw() override;

    void setRootView(View* platform_view) override;

    virtual void handleEvent(const Event& event) {}
    void setEventHandler(EventHandler event_handler) override;

private:
    class Impl;

    uint64_t window_id_;
    std::unique_ptr<Impl> impl_;

    void emitEvent(const Event& event);
};

} // namespace Grain::Platform
