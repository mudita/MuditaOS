// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "visitor/GuiVisitor.hpp"
#include <module-utils/json/json11.hpp>
#include <list>

namespace gui
{
    class ItemNode;
    class BoundingBox;
    class Color;

    class Item2JsonSerializingVisitor : public GuiVisitor
    {
        json11::Json::object sink;
        std::string itemName;

        auto serialize(gui::BoundingBox &box) -> json11::Json::array;
        auto serialize(gui::Color &color) -> json11::Json::array;

        void visit(gui::Item &item) override;
        void visit(gui::Rect &item) override;
        void visit(gui::Text &item) override;
        void visit(gui::Window &item) override;
        void visit(gui::Label &item) override;
        void visit(gui::BottomBar &item) override;
        void visit(gui::TopBar &item) override;

      public:
        [[nodiscard]] auto moveState() noexcept -> json11::Json::object
        {
            return std::move(sink);
        }
        [[nodiscard]] auto moveName() noexcept -> std::string
        {
            return std::move(itemName);
        }
    };
} // namespace gui
