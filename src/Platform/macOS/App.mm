#import <AppKit/AppKit.h>

#include "../Platform.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Grain::Platform {

class CocoaWindow final : public Window {
public:
    CocoaWindow(
        std::string_view title,
        int width,
        int height
    );

    ~CocoaWindow() override;

    void show() override;

    void setRootView(
        Grain::View* view
    ) override;

private:
    NSWindow* window_ = nil;

    Grain::View* root_view_ = nullptr;

    NSView* native_root_view_ = nil;
};


class CocoaApp final : public App {
public:
    CocoaApp();

    ~CocoaApp() override = default;

    std::unique_ptr<Window> createWindow(
        WindowId id,
        std::string_view title,
        int width,
        int height
    ) override;

    void run() override;

private:
    NSApplication* application_ = nil;

    std::vector<CocoaWindow*> windows_;
};


CocoaApp::CocoaApp()
{
    application_ = [NSApplication sharedApplication];

    [application_ setActivationPolicy:
        NSApplicationActivationPolicyRegular];
}


std::unique_ptr<Window> CocoaApp::createWindow(
    WindowId id,
    std::string_view title,
    int width,
    int height
)
{
    auto window =
        std::make_unique<CocoaWindow>(
            id,
            title,
            width,
            height
        );

    windows_.push_back(window.get());

    return window;
}


void CocoaApp::run()
{
    if (application_ == nil) {
        return;
    }

    [application_ activateIgnoringOtherApps:YES];
    [application_ run];
}


std::unique_ptr<App> createApp()
{
    return std::make_unique<CocoaApp>();
}

} // namespace Grain::Platform