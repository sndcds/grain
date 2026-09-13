#include <catch2/catch_test_macros.hpp>

#include "../../src/Platform/Linux/X11App.hpp"
#include "../../src/Platform/Linux/X11Window.hpp"

#include <vector>

TEST_CASE("X11: resize callbacks preserve application window IDs", "[Window]") {
    Grain::Platform::X11App app;
    const Grain::WindowId firstId = (Grain::WindowId{1} << 40) + 7;
    const Grain::WindowId secondId = firstId + 12;
    auto first = app.createWindow(firstId, "First", 100, 100);
    auto second = app.createWindow(secondId, "Second", 100, 100);
    auto* firstX11 = static_cast<Grain::Platform::X11Window*>(first.get());
    auto* secondX11 = static_cast<Grain::Platform::X11Window*>(second.get());
    REQUIRE(firstX11->id() == firstId);
    REQUIRE(secondX11->id() == secondId);

    std::vector<Grain::Event> events;
    app.setEventHandler([&](const Grain::Event& event) {
        if (event.type == Grain::EventType::WindowResize) {
            events.push_back(event);
            if (events.size() == 2) {
                app.quit();
            }
        }
    });

    // Unmapped windows avoid interference from any window manager.
    XResizeWindow(app.display(), secondX11->nativeWindow(), 240, 180);
    XResizeWindow(app.display(), firstX11->nativeWindow(), 320, 220);
    XFlush(app.display());
    app.run();

    REQUIRE(events.size() == 2);
    REQUIRE(events[0].windowId == secondId);
    REQUIRE(events[0].width == 240);
    REQUIRE(events[0].height == 180);
    REQUIRE(events[1].windowId == firstId);
    REQUIRE(events[1].width == 320);
    REQUIRE(events[1].height == 220);
}
