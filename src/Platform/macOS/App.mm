#import <AppKit/AppKit.h>

#include <grain/Platform/App.hpp>

#include <memory>
#include <vector>

namespace Grain {

class App::Impl {
public:
    NSApplication* application = nil;
    std::vector<std::unique_ptr<Window>> windows;
};

App& App::instance()
{
    static App app;
    return app;
}

App::App()
    : impl_(std::make_unique<Impl>())
{
    impl_->application = [NSApplication sharedApplication];

    [impl_->application setActivationPolicy:
        NSApplicationActivationPolicyRegular];
}

App::~App() = default;

Window* App::createWindow(
    std::string_view title,
    int width,
    int height
)
{
    auto window = std::unique_ptr<Window>(
        new Window(title, width, height)
    );

    Window* result = window.get();

    impl_->windows.push_back(std::move(window));

    result->show();

    return result;
}

void App::run()
{
    [impl_->application activateIgnoringOtherApps:YES];
    [impl_->application run];
}

} // namespace Grain