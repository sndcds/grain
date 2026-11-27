#pragma once

#include <grain/Platform/Platform.hpp>

#include <string_view>

namespace Grain::Platform {

class CocoaWindow final : public Window {
public:
    CocoaWindow(
        uint64_t id,
        std::string_view title,
        const Recti& rect,
        EventHandler event_handler
    );

    ~CocoaWindow() override;

    void show() override;

    [[nodiscard]]
    auto contentBounds() const -> Rectd override;

    void requestRedraw() override;

    void setRootView(View* platform_view) override;

    virtual void handleEvent(const Event& event) {}
    void setEventHandler(EventHandler event_handler) override;

    [[nodiscard]]
    auto nativeWindow() const -> void*;

private:
    class Impl;

    uint64_t window_id_;
    std::unique_ptr<Impl> impl_;

    void emitEvent(const Event& event);
};

} // namespace Grain::Platform
