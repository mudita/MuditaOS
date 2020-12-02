// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "DomNode.hpp"
#include <module-gui/gui/widgets/Item.hpp>

namespace gui::visitor
{
    using Node = std::unique_ptr<gui::dom::DomNode>;
    namespace item
    {
        constexpr inline auto itemType      = "ItemType";
        constexpr inline auto focus         = "Focus";
        constexpr inline auto visible       = "Visible";
        constexpr inline auto active        = "Active";
        constexpr inline auto childrenCount = "ChildrenCount";

        constexpr inline auto boundingBox = "BoundingBox";
        constexpr inline auto padding     = "Padding";
        constexpr inline auto margins     = "Margins";
        constexpr inline auto alignment   = "Alignment";

        constexpr inline auto x = "X";
        constexpr inline auto y = "Y";
        constexpr inline auto w = "W";
        constexpr inline auto h = "H";
    } // namespace item

    class ItemData
    {
      public:
        virtual void visit(std::list<Node> &nodes, int level, const Item &item)
        {
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, item::itemType, static_cast<int>(item.type)));
            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, item::focus, item.focus));
            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, item::visible, item.visible));
            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, item::active, item.activeItem));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, item::childrenCount, item.children.size()));

            auto addBoundingBox = [&nodes, level](const std::string &name, const BoundingBox &box) {
                nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, item::boundingBox, name));
                nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level + 1, item::x, box.x));
                nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level + 1, item::y, box.y));
                nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level + 1, item::w, box.w));
                nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level + 1, item::h, box.h));
            };

            addBoundingBox("widgetArea", item.widgetArea);
            addBoundingBox("widgetMinimumArea", item.widgetMinimumArea);
            addBoundingBox("widgetMaximumArea", item.widgetMaximumArea);
            addBoundingBox("drawArea", item.drawArea);
        }
    };

} // namespace gui::visitor
