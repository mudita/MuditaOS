// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Axes.hpp"
#include <cstdint>
#include <utility>

namespace gui
{
    using Length   = std::uint32_t;
    using Position = std::int32_t;

    struct Size
    {
        Length width = 0, height = 0;
        Size(Length w = 0, Length h = 0) : width(w), height(h)
        {}
        [[nodiscard]] auto get(Axis axis) const -> Length
        {
            return Axis::X == axis ? width : height;
        }
        [[nodiscard]] auto isZero() const -> bool
        {
            return 0 == width && 0 == height;
        }
    };

    struct Point
    {
        Position x = 0, y = 0;

        constexpr Point(Position x = 0, Position y = 0) : x(x), y(y)
        {}

        [[nodiscard]] constexpr auto get(Axis axis) -> Length
        {
            return Axis::X == axis ? x : y;
        }

        [[nodiscard]] constexpr auto isZero() -> bool
        {
            return 0 == x && 0 == y;
        }
    };

    enum class NavigationDirection
    {
        LEFT = 0x01,
        UP,
        RIGHT,
        DOWN,
        NONE,
    };

    enum class Status
    {
        GUI_SUCCESS = 0x01,
        GUI_FAILURE,
    };

    enum class RefreshModes
    {
        GUI_REFRESH_NONE,
        GUI_REFRESH_FAST = 1,
        GUI_REFRESH_DEEP
    };

    enum class ShowMode
    {
        GUI_SHOW_INIT = 0x01,
        GUI_SHOW_RETURN
    };

    enum class Orientation
    {
        Vertical,
        Horizontal
    };

    template <class T>
    bool operator&(const T &lhs, const T &rhs)
    {
        return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
    }
    template <class T>
    T operator|(const T &lhs, const T &rhs)
    {
        return static_cast<T>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }
    template <class T>
    T operator|=(const T &lhs, const T &rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    enum class RectangleEdge
    {
        None   = 0x00,
        Top    = 0x01,
        Bottom = 0x02,
        Left   = 0x04,
        Right  = 0x08,
        All    = 0x0F
    };

    enum class RectangleRoundedCorner
    {
        None        = 0x00,
        TopLeft     = 0x10,
        TopRight    = 0x20,
        BottomLeft  = 0x40,
        BottomRight = 0x80,
        All         = 0xF0,
    };

    enum class RectangleFlatEdge
    {
        None        = 0x00,
        TopLeft     = 0x01,
        TopRight    = 0x02,
        BottomLeft  = 0x04,
        BottomRight = 0x08
    };

    enum class RectangleYap
    {
        None        = 0x00,
        TopLeft     = 0x10,
        TopRight    = 0x20,
        BottomLeft  = 0x40,
        BottomRight = 0x80,
    };

    enum class Boundaries
    {
        Fixed,     ///< Fixed - will stop scrolling on first or last elements on appropriate top or bottom
                   ///< directions.
        Continuous ///< Continuous - will continue to beginning or end on first or last elements on
                   ///< appropriate top or bottom directions.
    };

    enum class ImageTypeSpecifier
    {
        None,
        W_G,
        B_G,
        W_M,
        B_M
    };
} // namespace gui

inline const char *c_str(gui::RefreshModes refresh)
{
    switch (refresh) {
    case gui::RefreshModes::GUI_REFRESH_NONE:
        return "GUI_REFRESH_NONE";
    case gui::RefreshModes::GUI_REFRESH_FAST:
        return "GUI_REFRESH_FAST";
    case gui::RefreshModes::GUI_REFRESH_DEEP:
        return "GUI_REFRESH_DEEP";
    }
    return "";
}
