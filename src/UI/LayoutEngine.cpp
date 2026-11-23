#include <grain/UI/LayoutEngine.hpp>
#include <grain/UI/Component.hpp>

#include <iostream>


namespace Grain {

void LayoutEngine::layout(
    View& root,
    const Rectd& bounds
) {
    sizingPass(root);
    preferredSizingPass(root);

    root.setLayoutBounds(bounds);

    growPass(root);
    positioningPass(root, bounds);
    roundingPass(root);
}


void LayoutEngine::sizingPass(View& root) {

    root.traversePostOrder(
        [](UIElement& element) -> bool {
            element.layoutSizing();
            return true;
        }
    );
}


void LayoutEngine::preferredSizingPass(View& root) {

    root.traverse(
            [](UIElement& element) -> bool {

                element.layoutPreferredSizing();

                element.layout_result_.allocated_size = {
                    element.layout_result_.preferred_size.width,
                    element.layout_result_.preferred_size.height
                };

                return true;
            }
        );
}


void LayoutEngine::growPass(View& root) {

    root.layoutGrow();
}


void LayoutEngine::positioningPass(View& root, const Rectd& bounds) {

    root.layoutPositioning(bounds);
}


void LayoutEngine::roundingPass(View& root) {

    root.traverse([](UIElement& element) -> bool {

        auto& bounds = element.layout_result_.bounds;

        // Round edges, not position and size independently.
        const double left = std::round(bounds.x);
        const double top = std::round(bounds.y);
        const double right = std::round(bounds.x + bounds.width);
        const double bottom = std::round(bounds.y + bounds.height);

        bounds.x = left;
        bounds.y = top;
        bounds.width = right - left;
        bounds.height = bottom - top;

        return true;
    });
}

}