// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DomNode.hpp"
#include <module-gui/gui/widgets/Rect.hpp>

namespace gui::visitor
{
    namespace rect
    {
        constexpr inline auto color         = "Color";
        constexpr inline auto intensity     = "Intensity";
        constexpr inline auto alpha         = "Alpha";
        constexpr inline auto penWidth      = "PenWidth";
        constexpr inline auto penFocusWidth = "PenFocusWidth";
        constexpr inline auto filled        = "Filled";
        constexpr inline auto edges         = "Edges";
        constexpr inline auto flatEdges     = "FlatEdges";
        constexpr inline auto corners       = "Corners";
        constexpr inline auto yaps          = "Yaps";
        constexpr inline auto yapSize       = "YapSize";
    } // namespace rect

    class RectData
    {
      public:
        virtual void visit(std::list<Node> &nodes, int level, const Rect &item)
        {
            auto addColor = [&nodes, level](const std::string &name, const Color &color) {
                nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, rect::color, name));
                nodes.emplace_back(
                    std::make_unique<gui::dom::DomAttributeNode>(level + 1, rect::intensity, color.intensity));
                nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level + 1, rect::alpha, color.alpha));
            };

            addColor("borderColor", item.borderColor);
            addColor("fillColor", item.fillColor);

            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, rect::penWidth, item.penWidth));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, rect::penFocusWidth, item.penFocusWidth));
            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, rect::filled, item.filled));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, rect::edges, static_cast<int>(item.edges)));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, rect::flatEdges, static_cast<int>(item.flatEdges)));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, rect::corners, static_cast<int>(item.corners)));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, rect::yaps, static_cast<int>(item.yaps)));
            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, rect::yapSize, item.yapSize));
        }
    };

} // namespace gui::visitor
