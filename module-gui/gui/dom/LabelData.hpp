// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DomNode.hpp"
#include <module-gui/gui/widgets/Label.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include <module-gui/gui/widgets/TopBar.hpp>
#include <module-gui/gui/widgets/BottomBar.hpp>
#include <module-gui/gui/widgets/Window.hpp>

namespace gui::visitor
{
    namespace label
    {

        constexpr inline auto text       = "Text";
        constexpr inline auto textLength = "TextLength";
        constexpr inline auto textHight  = "TextHight";
        constexpr inline auto textWidth  = "TextWidth";
    } // namespace label

    class LabelData
    {
      public:
        virtual void visit(std::list<Node> &nodes, int level, const gui::Label &item)
        {
            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, label::text, item.getText()));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, label::textLength, item.getTextLength()));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, label::textHight, item.getTextHeight()));
            nodes.emplace_back(
                std::make_unique<gui::dom::DomAttributeNode>(level, label::textWidth, item.getTextWidth()));
        }
    };

    class TextData
    {
      public:
        virtual void visit(std::list<Node> &nodes, int level, const gui::Text &item)
        {
            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, label::text, item.getText()));
        }
    };

    namespace window
    {
        constexpr inline auto name = "Name";

    }
    class WindowData
    {
      public:
        virtual void visit(std::list<Node> &nodes, int level, gui::Window &item)
        {
            nodes.emplace_back(std::make_unique<gui::dom::DomAttributeNode>(level, window::name, item.getName()));
        }
    };
} // namespace gui::visitor
