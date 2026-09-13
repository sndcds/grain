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
    explicit TestWindow(Grain::WindowId windowId) : id(windowId) {}

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
        requestedIds.push_back(id);
        if (failCreation) {
            return nullptr;
        }
        auto window = std::make_unique<TestWindow>(id);
        windows.push_back(window.get());
        return window;
    }

    void run() override { onRun(); }
    void quit() override { quitCalled = true; }

    using Grain::Platform::App::emitEvent;

    std::vector<Grain::WindowId> requestedIds;
    std::vector<TestWindow*> windows;
    std::function<void()> onRun;
    bool failCreation = false;
    bool quitCalled = false;
};

TestApp* platformApp = nullptr;

} // namespace

namespace Grain::Platform {

std::unique_ptr<App> createApp() {
    auto app = std::make_unique<TestApp>();
    platformApp = app.get();
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
    REQUIRE(platformApp->windows[0]->id == first->id());
    REQUIRE(platformApp->windows[1]->id == second->id());
    REQUIRE(platformApp->windows[0]->shown);
    REQUIRE(platformApp->windows[1]->shown);

    platformApp->failCreation = true;
    REQUIRE(app.createWindow("Failed", 100, 100) == nullptr);
    platformApp->failCreation = false;
    auto* third = app.createWindow("Third", 100, 100);
    REQUIRE(third != nullptr);
    REQUIRE(third->id() == second->id() + 2);
    REQUIRE(platformApp->requestedIds[2] == second->id() + 1);
    REQUIRE(platformApp->requestedIds[3] == third->id());
    REQUIRE(platformApp->windows[2]->id == third->id());

    // Drive the installed callback while the real App::run() is active.
    platformApp->onRun = [&] {
        Grain::Event event;
        event.windowId = platformApp->windows[0]->id;
        event.type = Grain::EventType::MouseMove;
        event.mouseX = 12.0;
        event.mouseY = 34.0;
        platformApp->emitEvent(event);
        REQUIRE(app.input().mousePosition().x == 12.0);
        REQUIRE(app.input().mousePosition().y == 34.0);

        event.windowId = platformApp->windows[1]->id;
        event.mouseX = 56.0;
        platformApp->emitEvent(event);
        REQUIRE(app.input().mousePosition().x == 56.0);

        event.type = Grain::EventType::MouseButtonDown;
        platformApp->emitEvent(event);
        REQUIRE(app.input().isMouseButtonDown(Grain::MouseButton::Left));
        event.type = Grain::EventType::MouseButtonUp;
        platformApp->emitEvent(event);
        REQUIRE_FALSE(app.input().isMouseButtonDown(Grain::MouseButton::Left));

        event.type = Grain::EventType::KeyDown;
        event.key = Grain::Key::A;
        platformApp->emitEvent(event);
        REQUIRE(app.input().isKeyDown(Grain::Key::A));
        event.type = Grain::EventType::KeyUp;
        platformApp->emitEvent(event);
        REQUIRE_FALSE(app.input().isKeyDown(Grain::Key::A));

        Grain::Event quit_event;
        quit_event.type = Grain::EventType::Quit; // Application-wide event.
        platformApp->emitEvent(quit_event);
        REQUIRE(platformApp->quitCalled);
    };
    app.run();
    platformApp->onRun = {};
}
