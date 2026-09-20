#pragma once

#include "../Platform.hpp"

namespace Grain::Platform {

class CocoaView final : public View {
public:
    explicit CocoaView(Grain::View* view);

    ~CocoaView() override;

    void requestRedraw() override;

    [[nodiscard]]
    void* nativeView() noexcept override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain::Platform