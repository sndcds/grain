#include <catch2/catch_test_macros.hpp>

#include <grain/Input/Input.hpp>

namespace Grain::Tests {

TEST_CASE("Input: key down", "[Input]") {
    Input input;

    Event event;
    event.type = EventType::KeyDown;
    event.key = Key::A;

    input.processEvent(event);

    REQUIRE(input.isKeyDown(Key::A));
    REQUIRE(input.wasKeyPressed(Key::A));
    REQUIRE_FALSE(input.wasKeyReleased(Key::A));
}

TEST_CASE("Input: key press is only reported once", "[Input]") {
    Input input;

    Event event;
    event.type = EventType::KeyDown;
    event.key = Key::A;

    input.processEvent(event);

    REQUIRE(input.isKeyDown(Key::A));
    REQUIRE(input.wasKeyPressed(Key::A));

    // A second KeyDown while the key is already down
    // must not create another "pressed" event.
    input.processEvent(event);

    REQUIRE(input.isKeyDown(Key::A));
    REQUIRE(input.wasKeyPressed(Key::A));
}

TEST_CASE("Input: beginFrame clears transient key state", "[Input]") {
    Input input;

    Event event;
    event.type = EventType::KeyDown;
    event.key = Key::A;

    input.processEvent(event);

    REQUIRE(input.isKeyDown(Key::A));
    REQUIRE(input.wasKeyPressed(Key::A));

    input.beginFrame();

    // The key is still physically down...
    REQUIRE(input.isKeyDown(Key::A));

    // ...but the press only happened in the previous frame.
    REQUIRE_FALSE(input.wasKeyPressed(Key::A));
    REQUIRE_FALSE(input.wasKeyReleased(Key::A));
}

TEST_CASE("Input: key up", "[Input]") {
    Input input;

    Event keyDown;
    keyDown.type = EventType::KeyDown;
    keyDown.key = Key::A;

    Event keyUp;
    keyUp.type = EventType::KeyUp;
    keyUp.key = Key::A;

    input.processEvent(keyDown);
    input.beginFrame();

    input.processEvent(keyUp);

    REQUIRE_FALSE(input.isKeyDown(Key::A));
    REQUIRE_FALSE(input.wasKeyPressed(Key::A));
    REQUIRE(input.wasKeyReleased(Key::A));
}

TEST_CASE("Input: beginFrame clears transient release state", "[Input]") {
    Input input;

    Event keyDown;
    keyDown.type = EventType::KeyDown;
    keyDown.key = Key::A;

    Event keyUp;
    keyUp.type = EventType::KeyUp;
    keyUp.key = Key::A;

    input.processEvent(keyDown);
    input.beginFrame();

    input.processEvent(keyUp);

    REQUIRE_FALSE(input.isKeyDown(Key::A));
    REQUIRE(input.wasKeyReleased(Key::A));

    input.beginFrame();

    REQUIRE_FALSE(input.isKeyDown(Key::A));
    REQUIRE_FALSE(input.wasKeyPressed(Key::A));
    REQUIRE_FALSE(input.wasKeyReleased(Key::A));
}

TEST_CASE("Input: mouse button down", "[Input]") {
    Input input;

    Event event;
    event.type = EventType::MouseButtonDown;
    event.mouseButton = MouseButton::Left;

    input.processEvent(event);

    REQUIRE(input.isMouseButtonDown(MouseButton::Left));
    REQUIRE(input.wasMouseButtonPressed(MouseButton::Left));
    REQUIRE_FALSE(
        input.wasMouseButtonReleased(MouseButton::Left)
    );
}

TEST_CASE("Input: mouse button up", "[Input]") {
    Input input;

    Event buttonDown;
    buttonDown.type = EventType::MouseButtonDown;
    buttonDown.mouseButton = MouseButton::Left;

    Event buttonUp;
    buttonUp.type = EventType::MouseButtonUp;
    buttonUp.mouseButton = MouseButton::Left;

    input.processEvent(buttonDown);
    input.beginFrame();

    input.processEvent(buttonUp);

    REQUIRE_FALSE(
        input.isMouseButtonDown(MouseButton::Left)
    );

    REQUIRE_FALSE(
        input.wasMouseButtonPressed(MouseButton::Left)
    );

    REQUIRE(
        input.wasMouseButtonReleased(MouseButton::Left)
    );
}

TEST_CASE("Input: beginFrame clears transient mouse state", "[Input]") {
    Input input;

    Event event;
    event.type = EventType::MouseButtonDown;
    event.mouseButton = MouseButton::Left;

    input.processEvent(event);

    REQUIRE(input.isMouseButtonDown(MouseButton::Left));
    REQUIRE(input.wasMouseButtonPressed(MouseButton::Left));

    input.beginFrame();

    REQUIRE(input.isMouseButtonDown(MouseButton::Left));
    REQUIRE_FALSE(
        input.wasMouseButtonPressed(MouseButton::Left)
    );
    REQUIRE_FALSE(
        input.wasMouseButtonReleased(MouseButton::Left)
    );
}

} // namespace Grain::Tests