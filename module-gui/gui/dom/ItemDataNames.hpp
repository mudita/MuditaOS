// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace gui::visitor
{

    enum Names
    {
        Item,
        Rect,
        Label,
        Text,
        StatusBar,
        BottomBar,
        Window,
        Children,
        ListItem
    };

    enum Item
    {
        ItemType,
        Focus,
        Visible,
        Active,
        ChildrenCount,
        WidgetArea,
        WidgetMinimumArea,
        WidgetMaximumArea,
        DrawArea,
        Padding,
        Margins,
        Alignment
    };

    enum Rect
    {
        BorderColor,
        FillColor,
        PenWidth,
        PenFocusWidth,
        Filled,
        Edges,
        FlatEdges,
        Corners,
        Yaps,
        YapSize,
    };

    enum Text
    {
        TextValue,
    };

    enum Window
    {
        WindowName
    };

} // namespace gui::visitor
