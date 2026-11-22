#include <grain/UI/LayoutEngine.hpp>
#include <grain/UI/Component.hpp>

#include <iostream>


namespace Grain {

void LayoutEngine::layout(
    View& root,
    const Rectd& bounds
) {
    sizingPass(root);

    // The root is controlled by its parent Window.
    root.setLayoutBounds(bounds);

    growPass(root);

    // Special positioning entry point for the root.
    root.layoutPositioning(bounds);
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
            return true;
        }
    );
}


void LayoutEngine::growPass(View& root) {

    root.traverse(
        [](UIElement& element) -> bool {
            element.layoutGrow();
            return true;
        }
    );
}


void LayoutEngine::positioningPass(View& root, const Rectd& bounds) {

    root.layoutPositioning(bounds);
}

}