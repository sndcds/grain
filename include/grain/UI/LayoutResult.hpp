#pragma once

namespace Grain {

struct LayoutResult {
    Rectd bounds;           // Final rectangle, including position
    Sized intrinsic_size;   // Natural content size
    Sized preferred_size;   // Size requested by the element's sizing rules
    Sized allocated_size;   // Size assigned by the parent during layout
};

}
