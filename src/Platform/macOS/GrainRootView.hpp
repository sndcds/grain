#pragma once

#import <AppKit/AppKit.h>

#include <functional>

#include <grain/Input/Event.hpp>

namespace Grain {

class View;

NSView* createGrainRootView(
    View* grainView,
    WindowId windowId,
    EventHandler eventHandler
);

} // namespace Grain
