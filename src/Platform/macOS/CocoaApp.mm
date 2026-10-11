#import <AppKit/AppKit.h>

#include "CocoaApp.hpp"
#include "CocoaWindow.hpp"
#include "CocoaView.hpp"

#include <memory>
#include <string_view>
#include <iostream>


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


bool CocoaApp::setIcon(const std::string& path) {
    std::cout << "CocoaApp::setIcon()" << std::endl;
    NSString* filePath =
        [NSString stringWithUTF8String:path.c_str()];

    std::cout << "2" << std::endl;
    NSImage* image =
        [[NSImage alloc] initWithContentsOfFile:filePath];
    std::cout << "3" << std::endl;

    if (image == nil) {
        return false;
    }
    std::cout << "4" << std::endl;

    [NSApplication sharedApplication].applicationIconImage = image;
    std::cout << "5" << std::endl;

    return true;
}


std::unique_ptr<Window> CocoaApp::createWindow(
    uint64_t id,
    std::string_view title,
    int width,
    int height
)
{
    std::cout << "CocoaApp::createWindow()" << std::endl;
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


void CocoaApp::run() {
    if (impl_->application == nil) {
        return;
    }

    [impl_->application
        activateIgnoringOtherApps:YES];

    [impl_->application run];
}


void CocoaApp::quit() {
    if (impl_->application == nil) {
        return;
    }

    [impl_->application terminate:nil];
}


std::unique_ptr<App> createApp() {
    return std::make_unique<CocoaApp>();
}

} // namespace Grain::Platform