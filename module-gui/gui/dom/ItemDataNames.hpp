// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace gui::visitor
{

    namespace names
    {
        constexpr inline auto item      = "Item";
        constexpr inline auto rect      = "Rect";
        constexpr inline auto label     = "Label";
        constexpr inline auto text      = "Text";
        constexpr inline auto topBar    = "TopBar";
        constexpr inline auto bottomBar = "BottomBar";
        constexpr inline auto window    = "Window";
    } // namespace names

    namespace item
    {
        constexpr inline auto itemType      = "ItemType";
        constexpr inline auto focus         = "Focus";
        constexpr inline auto visible       = "Visible";
        constexpr inline auto active        = "Active";
        constexpr inline auto childrenCount = "ChildrenCount";

        constexpr inline auto widgetArea        = "WidgetArea";
        constexpr inline auto widgetMinimumArea = "WidgetMinimumArea";
        constexpr inline auto widgetMaximumArea = "WidgetMaximumArea";
        constexpr inline auto drawArea          = "DrawArea";
        constexpr inline auto padding           = "Padding";
        constexpr inline auto margins           = "Margins";
        constexpr inline auto alignment         = "Alignment";

    } // namespace item

    namespace rect
    {
        constexpr inline auto borderColor   = "BorderColor";
        constexpr inline auto fillColor     = "FillColor";
        constexpr inline auto penWidth      = "PenWidth";
        constexpr inline auto penFocusWidth = "PenFocusWidth";
        constexpr inline auto filled        = "Filled";
        constexpr inline auto edges         = "Edges";
        constexpr inline auto flatEdges     = "FlatEdges";
        constexpr inline auto corners       = "Corners";
        constexpr inline auto yaps          = "Yaps";
        constexpr inline auto yapSize       = "YapSize";
    } // namespace rect

    namespace text
    {
        constexpr inline auto text       = "Text";
        constexpr inline auto textLength = "TextLength";
        constexpr inline auto textHight  = "TextHight";
        constexpr inline auto textWidth  = "TextWidth";
    } // namespace text

} // namespace gui::visitor
