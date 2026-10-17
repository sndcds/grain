#pragma once

#include "../Platform.hpp"

namespace Grain::Platform {

class CocoaView final : public View {
public:
    explicit CocoaView(Grain::View* view);

    ~CocoaView() override;

    void requestRedraw() override;

    void requestRedraw(const Rectd& dirty_rect) override;

    [[nodiscard]] void* nativeView() noexcept override;

    [[nodiscard]] Vec2d mousePos() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain::Platform