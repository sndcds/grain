#pragma once

#include <grain/Input/Event.hpp>
#include <grain/Input/Input.hpp>
#include <grain/Platform/Window.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Events/Emitter.hpp>

#include <sigslot/signal.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace Grain {

class App : public sigslot::observer_st {
public:
    static auto instance() -> App&;

    App(const App&) = delete;
    auto operator = (const App&) -> App& = delete;

    App(App&&) = delete;
    auto operator = (App&&) -> App& = delete;

    auto setIcon(const std::string& path) -> bool;


    //--------------------------------------------------------------------------
    //  Emitters
    //--------------------------------------------------------------------------

    Grain::Events::Emitter<> started;
    Grain::Events::Emitter<> stopping;

    //--------------------------------------------------------------------------
    //  Window
    //--------------------------------------------------------------------------

    auto addWindow(
        std::string tag,
        std::string_view title,
        const Recti& rect
    ) -> Window*;

    auto getWindow(std::string_view tag) -> Window*;
    auto getWindow(std::string_view tag) const -> const Window*;

    auto getWindowById(uint64_t id) -> Window*;


    //--------------------------------------------------------------------------
    //  Input
    //--------------------------------------------------------------------------

    [[nodiscard]] auto input() -> Input&;
    [[nodiscard]] auto input() const -> const Input&;


    //--------------------------------------------------------------------------
    //  Run
    //--------------------------------------------------------------------------

    void run();
    void quit();

private:
    App();
    ~App() = default;

    void processEvent(const Event& event);

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain