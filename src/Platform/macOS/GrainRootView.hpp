#pragma once

#import <AppKit/AppKit.h>

#include <functional>

#include <grain/Input/Event.hpp>

namespace Grain {

class View;

NSView* createGrainRootView(
    View* grainView,
    EventHandler eventHandler
);

} // namespace Grain