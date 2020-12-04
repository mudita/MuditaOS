// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GuiVisitor.hpp"
#include "ItemWalker.hpp"
#include <string>
#include <list>
#include <vector>

namespace gui
{
    class Item2PrettyPrintSerializingVisitor : public GuiVisitor, public ItemWalker
    {
        std::list<std::string> sink;
        void addLine(const std::string &name, const std::string &value);
        void addLine(const std::string &name, const std::vector<int> &value);
        void addLine(const std::string &name, int value);
        void addLine(const std::string &name, bool value);

        void visit(gui::Item &item) override;
        void visit(gui::Rect &item) override;
        void visit(gui::Text &item) override;
        void visit(gui::Window &item) override;
        void visit(gui::Label &item) override;
        void visit(gui::BottomBar &item) override;
        void visit(gui::TopBar &item) override;

      public:
        void traverse(gui::Item &root) override;
    };
} // namespace gui
