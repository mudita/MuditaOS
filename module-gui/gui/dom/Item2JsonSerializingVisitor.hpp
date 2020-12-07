// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "visitor/GuiVisitor.hpp"
#include "visitor/ItemWalker.hpp"
#include <module-utils/json/json11.hpp>
#include <list>

namespace gui
{
    class ItemNode;

    class Item2JsonSerializingVisitor : public GuiVisitor
    {
        json11::Json::array sink;
        std::string itemName;

        void visit(gui::Item &item) override;
        void visit(gui::Rect &item) override;
        void visit(gui::Text &item) override;
        void visit(gui::Window &item) override;
        void visit(gui::Label &item) override;
        void visit(gui::BottomBar &item) override;
        void visit(gui::TopBar &item) override;

      public:
        [[nodiscard]] auto getState() -> json11::Json::array
        {
            return std::move(sink);
        }
        [[nodiscard]] auto getName() -> std::string
        {
            return std::move(itemName);
        }
    };
} // namespace gui
