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
    root.traverseReverse(
        [](UIElement& element) -> bool {

            if (element.type() == UIElementType::View) {
                auto& view = static_cast<View&>(element);
                view.doSizing();
                // Calculate intrinsic view size
            }
            else {
                auto& component = static_cast<Component&>(element);
                component.doSizing();
                // Calculate intrinsic component size
            }

            return true;
        }
    );
}


void LayoutEngine::growPass(View& root) {
    root.traverse(
        [](UIElement& element) -> bool {

            if (element.type() == UIElementType::View) {
                auto& view = static_cast<View&>(element);
                // Process view growth
            }
            else {
                auto& component = static_cast<Component&>(element);
                // Process component growth
            }

            return true;
        }
    );
}


void LayoutEngine::positioningPass(View& root) {
    root.traverse(
        [](UIElement& element) -> bool {

            if (element.type() == UIElementType::View) {
                auto& view = static_cast<View&>(element);
                // Position child view
            }
            else {
                auto& component = static_cast<Component&>(element);
                // Position component
            }

            return true;
        }
    );
}

}