#pragma once

#import <AppKit/AppKit.h>

#include <functional>

#include <grain/Input/Event.hpp>

namespace Grain {

class View;

using EventHandler = std::function<void(const Event&)>;

NSView* createGrainRootView(
    View* grainView,
    EventHandler eventHandler
);

} // namespace Grain