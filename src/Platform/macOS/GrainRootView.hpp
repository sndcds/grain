#pragma once

#import <AppKit/AppKit.h>

namespace Grain {

class View;

/**
 * Creates the native macOS backing view for a Grain View.
 *
 * The returned NSView does not take ownership of grainView.
 * The Grain::Window remains responsible for the lifetime of the
 * Grain::View.
 */
NSView* createGrainRootView(View* grainView);

} // namespace Grain