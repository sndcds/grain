#include <grain/UI/LayoutEngine.hpp>
#include <grain/UI/Component.hpp>

#include <iostream>


namespace Grain {

void LayoutEngine::layout(View& root) {

    sizingPass(root);
    growPass(root);
    positioningPass(root);
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


void LayoutEngine::positioningPass(View& root) {

    root.layoutPositioning(Vec2d{0.0, 0.0});
}

}