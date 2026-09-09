#import <AppKit/AppKit.h>

#include "CocoaApp.hpp"
#include "CocoaWindow.hpp"

#include <memory>
#include <string_view>

namespace Grain::Platform {

class CocoaApp::Impl {
public:
    NSApplication* application = nil;
};


CocoaApp::CocoaApp()
    : impl_(std::make_unique<Impl>())
{
    impl_->application =
        [NSApplication sharedApplication];

    [impl_->application
        setActivationPolicy:
            NSApplicationActivationPolicyRegular];
}


CocoaApp::~CocoaApp() = default;


std::unique_ptr<Window> CocoaApp::createWindow(
    WindowId id,
    std::string_view title,
    int width,
    int height
)
{
    return std::make_unique<CocoaWindow>(
        id,
        title,
        width,
        height,
        [this](const Event& event) {
            emitEvent(event);
        }
    );
}


void CocoaApp::run()
{
    if (impl_->application == nil) {
        return;
    }

    [impl_->application
        activateIgnoringOtherApps:YES];

    [impl_->application run];
}


void CocoaApp::quit()
{
    if (impl_->application == nil) {
        return;
    }

    [impl_->application terminate:nil];
}


std::unique_ptr<App> createApp()
{
    return std::make_unique<CocoaApp>();
}

} // namespace Grain::Platform