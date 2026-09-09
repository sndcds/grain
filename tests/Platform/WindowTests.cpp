#include <catch2/catch_test_macros.hpp>

#include <grain/Platform/App.hpp>
#include <grain/Platform/Platform.hpp>

#include "../../src/Platform/Platform.hpp"

#include <functional>
#include <utility>
#include <vector>

namespace {

class TestWindow final : public Grain::Platform::Window {
public:
    explicit TestWindow(Grain::WindowId window_id) : id(window_id) {}

    void show() override { shown = true; }
    void setRootView(Grain::View*) override {}

    Grain::WindowId id;
    bool shown = false;
};

class TestApp final : public Grain::Platform::App {
public:
    std::unique_ptr<Grain::Platform::Window> createWindow(
        Grain::WindowId id,
        std::string_view,
        int,
        int
    ) override {
        requested_ids.push_back(id);
        if (fail_creation) {
            return nullptr;
        }
        auto window = std::make_unique<TestWindow>(id);
        windows.push_back(window.get());
        return window;
    }

    void run() override { on_run(); }
    void quit() override { quit_called = true; }

    using Grain::Platform::App::emitEvent;

    std::vector<Grain::WindowId> requested_ids;
    std::vector<TestWindow*> windows;
    std::function<void()> on_run;
    bool fail_creation = false;
    bool quit_called = false;
};

TestApp* platform_app = nullptr;

} // namespace

namespace Grain::Platform {

std::unique_ptr<App> createApp() {
    auto app = std::make_unique<TestApp>();
    platform_app = app.get();
    return app;
}

} // namespace Grain::Platform

TEST_CASE("App: window identity and platform event delivery", "[Window][Input]") {
    auto& app = Grain::App::instance();
    auto* first = app.createWindow("First", 100, 100);
    auto* second = app.createWindow("Second", 200, 200);
    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);
    REQUIRE(first->id() != 0);
    REQUIRE(second->id() == first->id() + 1);
    REQUIRE(platform_app->windows[0]->id == first->id());
    REQUIRE(platform_app->windows[1]->id == second->id());
    REQUIRE(platform_app->windows[0]->shown);
    REQUIRE(platform_app->windows[1]->shown);

    platform_app->fail_creation = true;
    REQUIRE(app.createWindow("Failed", 100, 100) == nullptr);
    platform_app->fail_creation = false;
    auto* third = app.createWindow("Third", 100, 100);
    REQUIRE(third != nullptr);
    REQUIRE(third->id() == second->id() + 2);
    REQUIRE(platform_app->requested_ids[2] == second->id() + 1);
    REQUIRE(platform_app->requested_ids[3] == third->id());
    REQUIRE(platform_app->windows[2]->id == third->id());

    // Drive the installed callback while the real App::run() is active.
    platform_app->on_run = [&] {
        Grain::Event event;
        event.window_id = platform_app->windows[0]->id;
        event.type = Grain::EventType::MouseMove;
        event.mouse_x = 12.0;
        event.mouse_y = 34.0;
        platform_app->emitEvent(event);
        REQUIRE(app.input().mousePosition().x == 12.0);
        REQUIRE(app.input().mousePosition().y == 34.0);

        event.window_id = platform_app->windows[1]->id;
        event.mouse_x = 56.0;
        platform_app->emitEvent(event);
        REQUIRE(app.input().mousePosition().x == 56.0);

        event.type = Grain::EventType::MouseButtonDown;
        platform_app->emitEvent(event);
        REQUIRE(app.input().isMouseButtonDown(Grain::MouseButton::Left));
        event.type = Grain::EventType::MouseButtonUp;
        platform_app->emitEvent(event);
        REQUIRE_FALSE(app.input().isMouseButtonDown(Grain::MouseButton::Left));

        event.type = Grain::EventType::KeyDown;
        event.key = Grain::Key::A;
        platform_app->emitEvent(event);
        REQUIRE(app.input().isKeyDown(Grain::Key::A));
        event.type = Grain::EventType::KeyUp;
        platform_app->emitEvent(event);
        REQUIRE_FALSE(app.input().isKeyDown(Grain::Key::A));

        Grain::Event quit_event;
        quit_event.type = Grain::EventType::Quit; // Application-wide event.
        platform_app->emitEvent(quit_event);
        REQUIRE(platform_app->quit_called);
    };
    app.run();
    platform_app->on_run = {};
}
