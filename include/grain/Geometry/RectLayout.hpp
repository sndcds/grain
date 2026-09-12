#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "grain/Geometry/Layout.hpp"
#include "grain/Geometry/Rect.hpp"
#include "../Math/Vec2.hpp"

namespace Grain {

template <typename T>
class RectLayout {
public:
    using RectType = Rect<T>;
    using Vec2Type = Vec2<T>;

    [[nodiscard]]
    static constexpr RectType fitRect(
        const RectType& source,
        const RectType& destination,
        FitMode mode
    ) noexcept
    {
        if (mode == FitMode::Stretch) {
            return destination;
        }

        if (mode == FitMode::Center) {
            return {
                destination.x + ((destination.width - source.width) / T{2}),
                destination.y + ((destination.height - source.height) / T{2}),
                source.width,
                source.height
            };
        }

        if (source.width == T{} || source.height == T{}) {
            return {};
        }

        const double widthScale =
            static_cast<double>(destination.width) /
            static_cast<double>(source.width);

        const double heightScale =
            static_cast<double>(destination.height) /
            static_cast<double>(source.height);

        const double scale =
            mode == FitMode::Cover
                ? std::max(widthScale, heightScale)
                : std::min(widthScale, heightScale);

        const T width =
            static_cast<T>(static_cast<double>(source.width) * scale);

        const T height =
            static_cast<T>(static_cast<double>(source.height) * scale);

        return {
            destination.x + ((destination.width - width) / T{2}),
            destination.y + ((destination.height - height) / T{2}),
            width,
            height
        };
    }

    [[nodiscard]]
    static constexpr RectType centeredSquare(
        const RectType& rect
    ) noexcept
    {
        const T size = std::min(
            std::abs(rect.width),
            std::abs(rect.height)
        );

        return {
            rect.x + ((rect.width - size) / T{2}),
            rect.y + ((rect.height - size) / T{2}),
            size,
            size
        };
    }

    [[nodiscard]]
    static constexpr RectType paddedRect(
        const RectType& rect,
        T top,
        T right,
        T bottom,
        T left
    ) noexcept
    {
        return {
            rect.x + left,
            rect.y + top,
            std::max(T{}, rect.width - left - right),
            std::max(T{}, rect.height - top - bottom)
        };
    }

    [[nodiscard]]
    static constexpr RectType rectInRectNormalized(
        const RectType& container,
        const RectType& rect,
        double horizontal,
        double vertical
    ) noexcept
    {
        const double x =
            static_cast<double>(container.x) +
            (horizontal *
                (static_cast<double>(container.width) -
                    static_cast<double>(rect.width)));

        const double y =
            static_cast<double>(container.y) +
            (vertical *
                (static_cast<double>(container.height) -
                    static_cast<double>(rect.height)));

        return {
            static_cast<T>(x),
            static_cast<T>(y),
            rect.width,
            rect.height
        };
    }

    [[nodiscard]]
    static constexpr Vec2Type alignedPos(
        const RectType& rect,
        Alignment alignment
    ) noexcept
    {
        switch (alignment) {
            case Alignment::TopLeft:
                return {rect.x, rect.y};

            case Alignment::Left:
                return {
                    rect.x,
                    rect.y + (rect.height / T{2})
                };

            case Alignment::BottomLeft:
                return {
                    rect.x,
                    rect.y + rect.height
                };

            case Alignment::Top:
                return {
                    rect.x + (rect.width / T{2}),
                    rect.y
                };

            case Alignment::Center:
                return {
                    rect.x + (rect.width / T{2}),
                    rect.y + (rect.height / T{2})
                };

            case Alignment::Bottom:
                return {
                    rect.x + (rect.width / T{2}),
                    rect.y + rect.height
                };

            case Alignment::TopRight:
                return {
                    rect.x + rect.width,
                    rect.y
                };

            case Alignment::Right:
                return {
                    rect.x + rect.width,
                    rect.y + (rect.height / T{2})
                };

            case Alignment::BottomRight:
                return {
                    rect.x + rect.width,
                    rect.y + rect.height
                };
        }

        return {rect.x, rect.y};
    }

    [[nodiscard]]
    static constexpr RectType cellRect(
        const RectType& rect,
        int columnCount,
        int rowCount,
        T columnSpacing,
        T rowSpacing,
        int columnIndex,
        int rowIndex,
        int columnSpan = 1,
        int rowSpan = 1,
        bool round = false
    ) noexcept
    {
        columnCount = std::max(columnCount, 1);
        rowCount = std::max(rowCount, 1);
        columnSpan = std::max(columnSpan, 1);
        rowSpan = std::max(rowSpan, 1);

        columnIndex = std::max(
            0,
            std::min(columnIndex, columnCount - 1)
        );

        rowIndex = std::max(
            0,
            std::min(rowIndex, rowCount - 1)
        );

        columnSpan = std::min(
            columnSpan,
            columnCount - columnIndex
        );

        rowSpan = std::min(
            rowSpan,
            rowCount - rowIndex
        );

        const T availableWidth =
            rect.width -
            columnSpacing * static_cast<T>(columnCount - 1);

        const T availableHeight =
            rect.height -
            rowSpacing * static_cast<T>(rowCount - 1);

        const T cellWidth =
            availableWidth / static_cast<T>(columnCount);

        const T cellHeight =
            availableHeight / static_cast<T>(rowCount);

        T x =
            rect.x +
            (cellWidth + columnSpacing) *
            static_cast<T>(columnIndex);

        T y =
            rect.y +
            (cellHeight + rowSpacing) *
            static_cast<T>(rowIndex);

        T width =
            cellWidth * static_cast<T>(columnSpan) +
            columnSpacing * static_cast<T>(columnSpan - 1);

        T height =
            cellHeight * static_cast<T>(rowSpan) +
            rowSpacing * static_cast<T>(rowSpan - 1);

        if (round) {
            x = static_cast<T>(std::round(x));
            y = static_cast<T>(std::round(y));
            width = static_cast<T>(std::round(width));
            height = static_cast<T>(std::round(height));
        }

        return {x, y, width, height};
    }

    [[nodiscard]]
    static constexpr RectType alignedRect(
        const RectType& container,
        Alignment alignment,
        T width,
        T height
    ) noexcept
    {
        T x = container.x;
        T y = container.y;

        switch (alignment) {
            case Alignment::TopLeft:
            case Alignment::Left:
            case Alignment::BottomLeft:
                x = container.x;
                break;

            case Alignment::Top:
            case Alignment::Center:
            case Alignment::Bottom:
                x = container.x + ((container.width - width) / T{2});
                break;

            case Alignment::TopRight:
            case Alignment::Right:
            case Alignment::BottomRight:
                x = container.x +
                    container.width - width;
                break;
        }

        switch (alignment) {
            case Alignment::TopLeft:
            case Alignment::Top:
            case Alignment::TopRight:
                y = container.y;
                break;

            case Alignment::Left:
            case Alignment::Center:
            case Alignment::Right:
                y = container.y + ((container.height - height) / T{2});
                break;

            case Alignment::BottomLeft:
            case Alignment::Bottom:
            case Alignment::BottomRight:
                y = container.y +
                    container.height - height;
                break;
        }

        return {x, y, width, height};
    }

    [[nodiscard]]
    static constexpr RectType edgeAlignedRectRelative(
        const RectType& rect,
        Alignment alignment,
        T top,
        T right,
        T bottom,
        T left
    ) noexcept
    {
        T x = left;
        T y = top;
        T width = rect.width - left - right;
        T height = rect.height - top - bottom;

        switch (alignment) {
            case Alignment::Left:
                x = left;
                width = std::max(T{}, right - left);
                break;

            case Alignment::Right:
                x = rect.width - right;
                width = std::max(T{}, right - left);
                break;

            case Alignment::Top:
                y = top;
                height = std::max(T{}, bottom - top);
                break;

            case Alignment::Bottom:
                y = rect.height - bottom;
                height = std::max(T{}, bottom - top);
                break;

            case Alignment::TopLeft:
                x = left;
                y = top;
                width = std::max(T{}, right - left);
                height = std::max(T{}, bottom - top);
                break;

            case Alignment::TopRight:
                x = rect.width - right;
                y = top;
                width = std::max(T{}, right - left);
                height = std::max(T{}, bottom - top);
                break;

            case Alignment::BottomLeft:
                x = left;
                y = rect.height - bottom;
                width = std::max(T{}, right - left);
                height = std::max(T{}, bottom - top);
                break;

            case Alignment::BottomRight:
                x = rect.width - right;
                y = rect.height - bottom;
                width = std::max(T{}, right - left);
                height = std::max(T{}, bottom - top);
                break;

            case Alignment::Center:
                break;
        }

        return {x, y, width, height};
    }

    [[nodiscard]]
    static constexpr RectType innerRect(
        const RectType& rect,
        T padding,
        T aspectRatio
    ) noexcept
    {
        const T availableWidth =
            std::max(T{}, rect.width - padding * T{2});

        const T availableHeight =
            std::max(T{}, rect.height - padding * T{2});

        if (aspectRatio <= T{} || availableWidth <= T{} ||
            availableHeight <= T{}) {
            return {
                rect.x + padding,
                rect.y + padding,
                availableWidth,
                availableHeight
            };
        }

        T width = availableWidth;
        T height = width / aspectRatio;

        if (height > availableHeight) {
            height = availableHeight;
            width = height * aspectRatio;
        }

        return {
            rect.x + ((rect.width - width) / T{2}),
            rect.y + ((rect.height - height) / T{2}),
            width,
            height
        };
    }

    [[nodiscard]]
    static constexpr RectType inset(
        const RectType& rect,
        T amount
    ) noexcept
    {
        return inset(rect, amount, amount, amount, amount);
    }

    [[nodiscard]]
    static constexpr RectType inset(
        const RectType& rect,
        T horizontal,
        T vertical
    ) noexcept
    {
        return inset(
            rect,
            vertical,
            horizontal,
            vertical,
            horizontal
        );
    }

    [[nodiscard]]
    static constexpr RectType inset(
        const RectType& rect,
        T top,
        T right,
        T bottom,
        T left
    ) noexcept
    {
        return {
            rect.x + left,
            rect.y + top,
            rect.width - left - right,
            rect.height - top - bottom
        };
    }

    [[nodiscard]]
    static constexpr RectType insetLeft(
        const RectType& rect,
        T amount
    ) noexcept
    {
        return {
            rect.x + amount,
            rect.y,
            rect.width - amount,
            rect.height
        };
    }

    [[nodiscard]]
    static constexpr RectType insetRight(
        const RectType& rect,
        T amount
    ) noexcept
    {
        return {
            rect.x,
            rect.y,
            rect.width - amount,
            rect.height
        };
    }

    [[nodiscard]]
    static constexpr RectType insetTop(
        const RectType& rect,
        T amount
    ) noexcept
    {
        return {
            rect.x,
            rect.y + amount,
            rect.width,
            rect.height - amount
        };
    }

    [[nodiscard]]
    static constexpr RectType insetBottom(
        const RectType& rect,
        T amount
    ) noexcept
    {
        return {
            rect.x,
            rect.y,
            rect.width,
            rect.height - amount
        };
    }

    [[nodiscard]]
    static constexpr RectType insetHorizontal(
        const RectType& rect,
        T amount
    ) noexcept
    {
        return inset(rect, amount, T{0});
    }

    [[nodiscard]]
    static constexpr RectType insetVertical(
        const RectType& rect,
        T amount
    ) noexcept
    {
        return inset(rect, T{0}, amount);
    }

    [[nodiscard]]
    static constexpr RectType expand(
        const RectType& rect,
        T amount
    ) noexcept
    {
        return {
            rect.x - amount,
            rect.y - amount,
            rect.width + amount * T{2},
            rect.height + amount * T{2}
        };
    }

    [[nodiscard]]
    static constexpr RectType setWidthFromCenter(
        const RectType& rect,
        T width
    ) noexcept
    {
        return {
            rect.x + ((rect.width - width) / T{2}),
            rect.y,
            width,
            rect.height
        };
    }

    [[nodiscard]]
    static constexpr RectType setWidthFromMax(
        const RectType& rect,
        T width
    ) noexcept
    {
        return {
            rect.x + rect.width - width,
            rect.y,
            width,
            rect.height
        };
    }

    [[nodiscard]]
    static constexpr RectType setHeightFromCenter(
        const RectType& rect,
        T height
    ) noexcept
    {
        return {
            rect.x,
            rect.y + ((rect.height - height) / T{2}),
            rect.width,
            height
        };
    }

    [[nodiscard]]
    static constexpr RectType setHeightFromMax(
        const RectType& rect,
        T height
    ) noexcept
    {
        return {
            rect.x,
            rect.y + rect.height - height,
            rect.width,
            height
        };
    }

    [[nodiscard]]
    static constexpr RectType setSizeFromCenter(
        const RectType& rect,
        T width,
        T height
    ) noexcept
    {
        return {
            rect.x + ((rect.width - width) / T{2}),
            rect.y + ((rect.height - height) / T{2}),
            width,
            height
        };
    }

    [[nodiscard]]
    static constexpr RectType translate(
        const RectType& rect,
        T x,
        T y
    ) noexcept
    {
        return {
            rect.x + x,
            rect.y + y,
            rect.width,
            rect.height
        };
    }

    [[nodiscard]]
    static constexpr RectType scale(
        const RectType& rect,
        T factor
    ) noexcept
    {
        return {
            rect.x * factor,
            rect.y * factor,
            rect.width * factor,
            rect.height * factor
        };
    }

    [[nodiscard]]
    static constexpr RectType scale(
        const RectType& rect,
        T xFactor,
        T yFactor
    ) noexcept
    {
        return {
            rect.x * xFactor,
            rect.y * yFactor,
            rect.width * xFactor,
            rect.height * yFactor
        };
    }

    [[nodiscard]]
    static constexpr RectType scaleSize(
        const RectType& rect,
        T factor
    ) noexcept
    {
        return {
            rect.x,
            rect.y,
            rect.width * factor,
            rect.height * factor
        };
    }

    [[nodiscard]]
    static constexpr RectType scaleSize(
        const RectType& rect,
        T xFactor,
        T yFactor
    ) noexcept
    {
        return {
            rect.x,
            rect.y,
            rect.width * xFactor,
            rect.height * yFactor
        };
    }

    [[nodiscard]]
    static constexpr RectType scaleCentered(
        const RectType& rect,
        T factor
    ) noexcept
    {
        const T width = rect.width * factor;
        const T height = rect.height * factor;

        return {
            rect.x + ((rect.width - width) / T{2}),
            rect.y + ((rect.height - height) / T{2}),
            width,
            height
        };
    }

    [[nodiscard]]
    static constexpr RectType alignInRect(
        const RectType& source,
        Alignment alignment,
        const RectType& container
    ) noexcept
    {
        return alignedRect(
            container,
            alignment,
            source.width,
            source.height
        );
    }

    [[nodiscard]]
    static constexpr RectType makePositiveSize(
        RectType rect
    ) noexcept
    {
        if (rect.width < T{}) {
            rect.x += rect.width;
            rect.width = -rect.width;
        }

        if (rect.height < T{}) {
            rect.y += rect.height;
            rect.height = -rect.height;
        }

        return rect;
    }

    [[nodiscard]]
    static constexpr RectType avoidNegativeSize(
        RectType rect
    ) noexcept
    {
        if (rect.width < T{}) {
            rect.width = T{};
        }

        if (rect.height < T{}) {
            rect.height = T{};
        }

        return rect;
    }

    [[nodiscard]]
    static constexpr RectType makeValidForArea(
        RectType rect,
        T width,
        T height
    ) noexcept
    {
        if (width <= T{} || height <= T{}) {
            return {};
        }

        rect = makePositiveSize(rect);

        if (rect.x < T{}) {
            rect.width += rect.x;
            rect.x = T{};
        }

        if (rect.y < T{}) {
            rect.height += rect.y;
            rect.y = T{};
        }

        if (rect.x >= width || rect.y >= height) {
            return {};
        }

        rect.width = std::min(
            rect.width,
            width - rect.x
        );

        rect.height = std::min(
            rect.height,
            height - rect.y
        );

        return avoidNegativeSize(rect);
    }
};

using RectLayoutd = RectLayout<double>;
using RectLayoutf = RectLayout<float>;
using RectLayouti = RectLayout<int>;

} // namespace Grain