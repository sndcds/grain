#include <catch2/catch_test_macros.hpp>

#include "../../src/Platform/macOS/GrainRootView.hpp"
#include <grain/UI/View.hpp>

#include <vector>

TEST_CASE("Cocoa: mouse events retain their root view window ID", "[Window][Input]") {
    @autoreleasepool {
        Grain::View view;
        std::vector<Grain::Event> events;
        auto handler = [&](const Grain::Event& event) {
            events.push_back(event);
        };
        const Grain::WindowId first_id = (Grain::WindowId{1} << 40) + 7;
        const Grain::WindowId second_id = first_id + 12;
        NSView* first = Grain::createGrainRootView(&view, first_id, handler);
        NSView* second = Grain::createGrainRootView(&view, second_id, handler);
        REQUIRE(first != nil);
        REQUIRE(second != nil);

        // No NSWindow is attached: identity must come from the factory argument.
        // A nil native event supplies zero coordinates for these identity checks.
        for (NSView* native_view : {second, first}) {
            [native_view mouseDown:nil];
            [native_view mouseUp:nil];
            [native_view mouseDragged:nil];
            [native_view mouseMoved:nil];
        }
        [first release];
        [second release];

        REQUIRE(events.size() == 8);
        for (std::size_t i = 0; i < events.size(); ++i) {
            REQUIRE(events[i].window_id == (i < 4 ? second_id : first_id));
        }
        for (std::size_t offset : {0u, 4u}) {
            REQUIRE(events[offset].type == Grain::EventType::MouseButtonDown);
            REQUIRE(events[offset + 1].type == Grain::EventType::MouseButtonUp);
            REQUIRE(events[offset + 2].type == Grain::EventType::MouseMove);
            REQUIRE(events[offset + 3].type == Grain::EventType::MouseMove);
        }
    }
}
