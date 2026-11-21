#pragma once

#include <grain/UI/View.hpp>

namespace Grain {

class LayoutEngine {
public:
    void layout(View& root);

private:
    void sizingPass(View& root);
    void growPass(View& root);
    void positioningPass(View& root);
};

}