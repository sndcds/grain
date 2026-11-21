#include <grain/UI/UIElement.hpp>
#include <grain/UI/View.hpp>

namespace Grain {

auto UIElement::parentView() const -> View* {
    return static_cast<View*>(parent_);
}

}