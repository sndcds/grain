
#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Size.hpp>

namespace Grain {

//------------------------------------------------------------------------------
//  Popup placement
//------------------------------------------------------------------------------

enum class PopupPlacement {
    Below,
    Above,
    Left,
    Right
};

enum class PopupAlignment {
    Start,
    Center,
    End
};


//------------------------------------------------------------------------------
//  Popup options
//------------------------------------------------------------------------------

struct PopupOptions {
    PopupPlacement placement = PopupPlacement::Below;
    PopupAlignment alignment = PopupAlignment::Start;

    double offset_x = 0.0;
    double offset_y = 4.0;

    bool accepts_keyboard_focus = false;
    bool dismiss_on_outside_click = true;
};

} // namespace Grain
