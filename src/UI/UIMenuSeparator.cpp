#include <grain/UI/UIMenuSeparator.hpp>

namespace Grain {

void UIMenuSeparator::draw(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    (void)dirty_rect;

    const Rectd item_bounds = bounds();

    gc.setStrokeColor({0.82f, 0.82f, 0.82f, 1.0f});
    gc.setStrokeWidth(1.0);

    const double y =
        item_bounds.y + item_bounds.height / 2.0;

    gc.beginPath();
    gc.moveTo({item_bounds.x + 8.0, y});
    gc.lineTo({item_bounds.right() - 8.0, y});
    gc.strokePath();
}


void UIMenuSeparator::layoutSizing() {
    layout_result_.intrinsic_size = {0.0, 9.0};
    layoutPreferredSizing();
}

} // namespace Grain