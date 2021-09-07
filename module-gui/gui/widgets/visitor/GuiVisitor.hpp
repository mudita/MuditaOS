// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace gui
{
    class Item;
    class Rect;
    class Text;
    class Window;
    class Label;
    class BottomBar;
    class ListItem;

    namespace status_bar
    {
        class StatusBar;
    }

    /// The general purpose abstract interface for enabling Double-Dispatch behavior throughout `gui::Item`'s
    /// inheritance hierarchy.
    class GuiVisitor
    {
      public:
        virtual void visit(gui::Item &item)                  = 0;
        virtual void visit(gui::Rect &item)                  = 0;
        virtual void visit(gui::Text &item)                  = 0;
        virtual void visit(gui::Window &item)                = 0;
        virtual void visit(gui::Label &item)                 = 0;
        virtual void visit(gui::BottomBar &item)             = 0;
        virtual void visit(gui::status_bar::StatusBar &item) = 0;
        virtual void visit(gui::ListItem &item)              = 0;
        virtual ~GuiVisitor()                                = default;
    };
} // namespace gui
