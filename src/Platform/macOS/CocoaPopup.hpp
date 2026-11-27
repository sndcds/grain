#pragma once

#include <grain/Platform/Platform.hpp>

namespace Grain::Platform {

class CocoaPopup final : public Popup {
public:
    CocoaPopup();
    ~CocoaPopup() override;

    void show(
        Window& parent,
        View* content,
        const Rectd& anchor,
        const Sized& size,
        const PopupOptions& options
    ) override;

    void hide() override;

    void reposition(const Rectd& anchor) override;
    void resize(const Sized& size) override;

    [[nodiscard]]
    auto isVisible() const -> bool override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain::Platform