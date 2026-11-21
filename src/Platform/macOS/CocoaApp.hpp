#pragma once

#include <grain/Platform/Platform.hpp>

#include <memory>
#include <string_view>


namespace Grain::Platform {

class CocoaApp final : public App {
public:
    CocoaApp();

    ~CocoaApp() override;

    bool setIcon(const std::string& path) override;

    std::unique_ptr<Window> addWindow(
        uint64_t id,
        std::string_view title,
        const Recti& rect
        ) override;

    void run() override;

    void quit() override;

private:
    class Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace Grain::Platform