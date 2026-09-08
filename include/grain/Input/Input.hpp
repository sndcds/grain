#pragma once

#include "grain/Input/Event.hpp"

namespace Grain {

class Input {
public:
    bool isKeyDown(Key key) const noexcept;

    bool wasKeyPressed(Key key) const noexcept;

    bool wasKeyReleased(Key key) const noexcept;

    bool isMouseButtonDown(MouseButton button) const noexcept;

    bool wasMouseButtonPressed(MouseButton button) const noexcept;

    bool wasMouseButtonReleased(MouseButton button) const noexcept;

    Vec2d mousePosition() const noexcept;

    Vec2d mouseDelta() const noexcept;

    void beginFrame() noexcept;

    void processEvent(const Event& event) noexcept;

private:
    // implementation
};

} // namespace Grain