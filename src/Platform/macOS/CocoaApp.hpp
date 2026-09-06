#pragma once

#include "../Platform.hpp"

#include <memory>
#include <string_view>

namespace Grain::Platform {

class CocoaApp final : public App {
public:
    CocoaApp();

    ~CocoaApp() override;

    std::unique_ptr<Window> createWindow(
        std::string_view title,
        int width,
        int height
        ) override;

    void run() override;

private:
    class Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace Grain::Platform