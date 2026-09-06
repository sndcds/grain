#pragma once

namespace Grain {

template <typename T>
struct Rect {
    T x = T{};
    T y = T{};
    T width = T{};
    T height = T{};
};

using Rectd = Rect<double>;
using Rectf = Rect<float>;
using Recti = Rect<int>;

} // namespace Grain