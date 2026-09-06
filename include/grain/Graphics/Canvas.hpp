#pragma once

#include <grain/Graphics/Color.hpp>

namespace Grain {

class Canvas {
public:
    struct Rect {
        double x = 0.0;
        double y = 0.0;
        double width = 0.0;
        double height = 0.0;
    };

    virtual ~Canvas() = default;

    Canvas(const Canvas&) = delete;

    Canvas& operator=(const Canvas&) = delete;

    Canvas(Canvas&&) = delete;

    Canvas& operator=(Canvas&&) = delete;

    virtual void clear(Color color) = 0;

    virtual void fillRect(
        const Rect& rect,
        Color color
        ) = 0;

protected:
    Canvas() = default;
};

} // namespace Grain