// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GuiVisitor.hpp"
#include "ItemWalker.hpp"
#include <module-utils/json/json11.hpp>
#include <list>

namespace gui
{
    class ItemNode;

    class Item2JsonSerializingVisitor : public GuiVisitor, public ItemWalker
    {
        using document = std::map<int, std::list<json11::Json::object>>;
        json11::Json::object sink;

        void visit(gui::Item &item) override;
        void visit(gui::Rect &item) override;
        void visit(gui::Text &item) override;
        void visit(gui::Window &item) override;
        void visit(gui::Label &item) override;
        void visit(gui::BottomBar &item) override;
        void visit(gui::TopBar &item) override;

        void handleSibling(gui::ItemNode &node, document &doc, int &level);
        void handleParent(gui::ItemNode &node, document &doc, int &level);
        void handleOther(gui::ItemNode &node, document &doc, int &level);

      public:
        void traverse(gui::Item &root) override;
    };
} // namespace gui
