// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GuiVisitor.hpp"
#include "DomNode.hpp"
#include <memory>
namespace gui
{
    class DOMDataHandler;

    class GuiDOMVisitor : public GuiVisitor
    {
        DOMDataHandler *handler;

      public:
        GuiDOMVisitor();
        ~GuiDOMVisitor();
        void visit(gui::Item &item) override;
        void visit(gui::Rect &item) override;
        void visit(gui::Text &item) override;
        void visit(gui::Window &item) override;
        void visit(gui::Label &item) override;
        void visit(gui::BottomBar &item) override;
        void visit(gui::TopBar &item) override;

        void dumpDOM();
    };

} // namespace gui
