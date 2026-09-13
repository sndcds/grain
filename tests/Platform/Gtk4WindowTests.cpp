#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "../../src/Platform/LinuxGTK4/Gtk4App.hpp"
#include "../../src/Platform/LinuxGTK4/Gtk4Window.hpp"
#include "../../src/Platform/LinuxGTK4/Gtk4GraphicContext.hpp"

#include <grain/UI/View.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

class PaintedView final : public Grain::View {
public:
    int draws = 0;
    bool nativePaths = false;

    void draw(Grain::GraphicContext& context) override {
        ++draws;
        nativePaths = dynamic_cast<Grain::Platform::Gtk4GraphicContext*>(&context) != nullptr;
        context.setFillColor({1, 0, 0});
        context.fillRect(bounds());
        Grain::GraphicPath path;
        path.addPoint(8, 8);
        path.addPoint(24, 8);
        path.addPoint(24, 24);
        path.setClosed(true);
        context.setStrokeColor({0, 0, 1});
        context.strokePath(path);
    }
};

// Bound every event-loop test even when a regression prevents drawing or closing.
class Watchdog {
public:
    explicit Watchdog(Grain::Platform::App& app) : app_(app) {
        source_ = g_timeout_add(5000, +[](gpointer data) -> gboolean {
            auto& self = *static_cast<Watchdog*>(data);
            self.expired = true;
            self.source_ = 0;
            self.app_.quit();
            return G_SOURCE_REMOVE;
        }, this);
    }
    ~Watchdog() {
        if (source_ != 0) {
            g_source_remove(source_);
        }
    }
    bool expired = false;

private:
    Grain::Platform::App& app_;
    guint source_ = 0;
};

} // namespace

TEST_CASE("GTK4: factory renders views, preserves IDs, and exits after the last close", "[GTK4]") {
    auto app = Grain::Platform::createApp();
    REQUIRE(dynamic_cast<Grain::Platform::Gtk4App*>(app.get()) != nullptr);
    PaintedView firstView;
    PaintedView secondView;
    const Grain::WindowId firstId = (Grain::WindowId{1} << 40) + 7;
    const Grain::WindowId secondId = firstId + 12;
    auto first = app->createWindow(firstId, "First", 160, 120);
    auto second = app->createWindow(secondId, "Second", 240, 180);
    auto& firstGtk = *static_cast<Grain::Platform::Gtk4Window*>(first.get());
    auto& secondGtk = *static_cast<Grain::Platform::Gtk4Window*>(second.get());
    REQUIRE(firstGtk.id() == firstId);
    REQUIRE(secondGtk.id() == secondId);
    REQUIRE(std::string(gtk_window_get_title(firstGtk.nativeWindow())) == "First");
    first->setRootView(&firstView);
    second->setRootView(&secondView);
    std::vector<Grain::Event> events;
    app->setEventHandler([&](const Grain::Event& event) { events.push_back(event); });
    first->show();
    second->show();

    struct Progress {
        PaintedView& firstView;
        PaintedView& secondView;
        Grain::Platform::Gtk4Window& first;
        Grain::Platform::Gtk4Window& second;
        int stage = 0;
        bool remainingWindow = false;
    } progress{firstView, secondView, firstGtk, secondGtk};

    const guint poll = g_timeout_add(10, +[](gpointer data) -> gboolean {
        auto& p = *static_cast<Progress*>(data);
        if (p.stage == 0 && p.firstView.draws > 0 && p.secondView.draws > 0) {
            gtk_window_close(p.first.nativeWindow());
            p.stage = 1;
        } else if (p.stage == 1) {
            p.remainingWindow = gtk_widget_get_visible(GTK_WIDGET(p.second.nativeWindow()));
            gtk_window_close(p.second.nativeWindow());
            p.stage = 2;
        }
        return G_SOURCE_CONTINUE;
    }, &progress);
    Watchdog watchdog(*app);
    app->run();
    g_source_remove(poll);

    REQUIRE_FALSE(watchdog.expired);
    REQUIRE(progress.stage == 2);
    REQUIRE(progress.remainingWindow);
    REQUIRE(firstView.nativePaths);
    REQUIRE(secondView.nativePaths);
    REQUIRE(firstView.bounds().width == 160);
    REQUIRE(firstView.bounds().height == 120);
    REQUIRE(secondView.bounds().width == 240);
    REQUIRE(secondView.bounds().height == 180);
    for (const auto id : {firstId, secondId}) {
        REQUIRE(std::ranges::any_of(events, [id](const Grain::Event& event) {
            return event.type == Grain::EventType::WindowResize && event.windowId == id
                && event.width > 0 && event.height > 0;
        }));
        REQUIRE(std::ranges::count_if(events, [id](const Grain::Event& event) {
            return event.type == Grain::EventType::WindowClose && event.windowId == id;
        }) == 1);
    }
    // Closed native windows remain safe until their Grain wrappers are destroyed.
    first->setRootView(nullptr);
    first->show();
}

TEST_CASE("GTK4: drawing exceptions return through App run", "[GTK4]") {
    Grain::Platform::Gtk4App app;
    class ThrowingView final : public Grain::View {
        void draw(Grain::GraphicContext&) override {
            throw std::runtime_error("drawing failed");
        }
    } view;
    auto window = app.createWindow(1, "Exception", 100, 100);
    window->setRootView(&view);
    window->show();
    Watchdog watchdog(app);
    REQUIRE_THROWS_WITH(app.run(), "drawing failed");
    REQUIRE_FALSE(watchdog.expired);
}

TEST_CASE("GTK4: windows can be destroyed before run or outlive the platform app", "[GTK4]") {
    std::unique_ptr<Grain::Platform::Window> survivor;
    int events = 0;
    {
        Grain::Platform::Gtk4App app;
        app.setEventHandler([&](const Grain::Event&) { ++events; });
        auto discarded = app.createWindow(1, "Discarded", 100, 100);
        discarded.reset();
        survivor = app.createWindow(2, "Survivor", 100, 100);
    }
    const int before = events;
    auto& window = *static_cast<Grain::Platform::Gtk4Window*>(survivor.get());
    gtk_widget_allocate(gtk_window_get_child(window.nativeWindow()), 200, 150, -1, nullptr);
    REQUIRE(events == before);
    survivor.reset();
}
