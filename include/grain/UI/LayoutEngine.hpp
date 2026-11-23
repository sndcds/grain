#pragma once

#include <grain/UI/View.hpp>

namespace Grain {

class LayoutEngine {
public:
    void layout(View& root, const Rectd& bounds);

private:
    void sizingPass(View& root);
    void preferredSizingPass(View& root);
    void growPass(View& root);
    void positioningPass(View& root, const Rectd& bounds);
    static void roundingPass(View& root);
};

}