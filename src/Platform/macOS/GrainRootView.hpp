#pragma once

#import <AppKit/AppKit.h>

#include <grain/Input/Event.hpp>

#include <cstdint>


namespace Grain {

class View;

NSView* createGrainRootView(View* grain_view);

} // namespace Grain