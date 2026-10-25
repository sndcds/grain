#include <catch2/catch_test_macros.hpp>

#include <grain/Input/Input.hpp>

namespace Grain::Tests {

TEST_CASE("Input: key down", "[Input]") {
    Input input;

    Event event;
    event.type_ = EventType::KeyDown;
    event.key_ = KeyCode::A;

    input.processEvent(event);

    REQUIRE(input.isKeyDown(KeyCode::A));
    REQUIRE(input.wasKeyPressed(KeyCode::A));
    REQUIRE_FALSE(input.wasKeyReleased(KeyCode::A));
}

TEST_CASE("Input: key press is only reported once", "[Input]") {
    Input input;

    Event event;
    event.type_ = EventType::KeyDown;
    event.key_ = KeyCode::A;

    input.processEvent(event);

    REQUIRE(input.isKeyDown(KeyCode::A));
    REQUIRE(input.wasKeyPressed(KeyCode::A));

    // Repeated KeyDown events must not create another press transition.
    input.processEvent(event);

    REQUIRE(input.isKeyDown(KeyCode::A));
    REQUIRE(input.wasKeyPressed(KeyCode::A));
}

TEST_CASE("Input: beginFrame clears transient key state", "[Input]") {
    Input input;

    Event event;
    event.type_ = EventType::KeyDown;
    event.key_ = KeyCode::A;

    input.processEvent(event);

    REQUIRE(input.isKeyDown(KeyCode::A));
    REQUIRE(input.wasKeyPressed(KeyCode::A));

    input.beginFrame();

    REQUIRE(input.isKeyDown(KeyCode::A));
    REQUIRE_FALSE(input.wasKeyPressed(KeyCode::A));
    REQUIRE_FALSE(input.wasKeyReleased(KeyCode::A));
}

TEST_CASE("Input: key up", "[Input]") {
    Input input;

    Event key_down;
    key_down.type_ = EventType::KeyDown;
    key_down.key_ = KeyCode::A;

    Event key_up;
    key_up.type_ = EventType::KeyUp;
    key_up.key_ = KeyCode::A;

    input.processEvent(key_down);
    input.beginFrame();
    input.processEvent(key_up);

    REQUIRE_FALSE(input.isKeyDown(KeyCode::A));
    REQUIRE_FALSE(input.wasKeyPressed(KeyCode::A));
    REQUIRE(input.wasKeyReleased(KeyCode::A));
}

TEST_CASE("Input: beginFrame clears transient release state", "[Input]") {
    Input input;

    Event key_down;
    key_down.type_ = EventType::KeyDown;
    key_down.key_ = KeyCode::A;

    Event key_up;
    key_up.type_ = EventType::KeyUp;
    key_up.key_ = KeyCode::A;

    input.processEvent(key_down);
    input.beginFrame();
    input.processEvent(key_up);

    REQUIRE_FALSE(input.isKeyDown(KeyCode::A));
    REQUIRE(input.wasKeyReleased(KeyCode::A));

    input.beginFrame();

    REQUIRE_FALSE(input.isKeyDown(KeyCode::A));
    REQUIRE_FALSE(input.wasKeyPressed(KeyCode::A));
    REQUIRE_FALSE(input.wasKeyReleased(KeyCode::A));
}

TEST_CASE("Input: mouse button down", "[Input]") {
    Input input;

    Event event;
    event.type_ = EventType::MouseButtonDown;
    event.mouse_button_ = MouseButton::Left;

    input.processEvent(event);

    REQUIRE(input.isMouseButtonDown(MouseButton::Left));
    REQUIRE(input.wasMouseButtonPressed(MouseButton::Left));
    REQUIRE_FALSE(input.wasMouseButtonReleased(MouseButton::Left));
}

TEST_CASE("Input: mouse button up", "[Input]") {
    Input input;

    Event button_down;
    button_down.type_ = EventType::MouseButtonDown;
    button_down.mouse_button_ = MouseButton::Left;

    Event button_up;
    button_up.type_ = EventType::MouseButtonUp;
    button_up.mouse_button_ = MouseButton::Left;

    input.processEvent(button_down);
    input.beginFrame();
    input.processEvent(button_up);

    REQUIRE_FALSE(input.isMouseButtonDown(MouseButton::Left));
    REQUIRE_FALSE(input.wasMouseButtonPressed(MouseButton::Left));
    REQUIRE(input.wasMouseButtonReleased(MouseButton::Left));
}

TEST_CASE("Input: beginFrame clears transient mouse state", "[Input]") {
    Input input;

    Event event;
    event.type_ = EventType::MouseButtonDown;
    event.mouse_button_ = MouseButton::Left;

    input.processEvent(event);

    REQUIRE(input.isMouseButtonDown(MouseButton::Left));
    REQUIRE(input.wasMouseButtonPressed(MouseButton::Left));

    input.beginFrame();

    REQUIRE(input.isMouseButtonDown(MouseButton::Left));
    REQUIRE_FALSE(input.wasMouseButtonPressed(MouseButton::Left));
    REQUIRE_FALSE(input.wasMouseButtonReleased(MouseButton::Left));
}

} // namespace Grain::Tests