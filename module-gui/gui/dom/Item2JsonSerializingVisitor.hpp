// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "visitor/GuiVisitor.hpp"
#include <json11.hpp>
#include <list>

namespace gui
{
    class ItemNode;
    class BoundingBox;
    class Color;

    /// Visitor serializing a single object of a class in a `gui::Item`'s inheritance hierarchy into the JSON format. On
    /// each visit, acceptor is serialized with a result appended into the `sink`.
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
        void visit(gui::status_bar::StatusBar &item) override;
        void visit(gui::ListItem &item) override;

      public:
        /// retrieves current state of the `sink`. The state of the `sink` after call is default-initialized
        [[nodiscard]] auto moveState() noexcept -> json11::Json::object
        {
            return std::move(sink);
        }
        /// retrieves name of a object's type that was serialized.
        [[nodiscard]] auto moveName() noexcept -> std::string
        {
            return std::move(itemName);
        }
    };
} // namespace gui
