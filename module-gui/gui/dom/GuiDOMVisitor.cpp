// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GuiDOMVisitor.hpp"
#include "DomNode.hpp"
#include "ItemData.hpp"
#include "RectData.hpp"
#include "LabelData.hpp"
#include <module-gui/gui/widgets/Item.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include <module-apps/windows/AppWindow.hpp>

#include <iostream>
using namespace gui;

namespace gui
{
    namespace names
    {
        constexpr inline auto item      = "Item";
        constexpr inline auto rect      = "Rect";
        constexpr inline auto label     = "Label";
        constexpr inline auto text      = "Text";
        constexpr inline auto topBar    = "TopBar";
        constexpr inline auto bottomBar = "BottomBar";
    } // namespace names

    using node = std::unique_ptr<gui::dom::DomNode>;
    class DOMDataHandler
    {
        GuiDOMVisitor &visitor;

        int level = 0;
        std::list<node> nodes;

      public:
        DOMDataHandler(GuiDOMVisitor &visitor) : visitor(visitor)
        {}

        void onNode(const std::string &typeName) noexcept;
        void onNodeEnd() noexcept;
        void handle(Item &item)
        {
            gui::visitor::ItemData dataVisitor;
            dataVisitor.visit(nodes, level, item);
            for (Item *el : item.children) {
                if (&*el == nullptr) {
                    continue;
                }
                el->accept(visitor);
            }
        }
        void handle(Rect &item)
        {
            gui::visitor::RectData dataVisitor;
            dataVisitor.visit(nodes, level, item);
            handle(static_cast<Item &>(item));
        }
        void handle(Label &item)
        {
            gui::visitor::LabelData dataVisitor;
            dataVisitor.visit(nodes, level, item);
            handle(static_cast<Rect &>(item));
        }
        void handle(gui::Text &item)
        {
            gui::visitor::TextData dataVisitor;
            dataVisitor.visit(nodes, level, item);
            handle(static_cast<Rect &>(item));
        }
        void handle(gui::Window &item)
        {
            gui::visitor::WindowData dataVisitor;
            dataVisitor.visit(nodes, level, item);
            handle(static_cast<Item &>(item));
        }

        std::list<node> &get() noexcept
        {
            return nodes;
        }
    };
} // namespace gui

void DOMDataHandler::onNode(const std::string &typeName) noexcept
{
    nodes.emplace_back(std::make_unique<gui::dom::DomChildNode>(level, typeName));
    nodes.emplace_back(
        std::make_unique<gui::dom::DomFunctionalNode>(level, gui::dom::DomFunctionalNode::Type::NodeBegin));
    level++;
}
void DOMDataHandler::onNodeEnd() noexcept
{
    level--;
    nodes.emplace_back(
        std::make_unique<gui::dom::DomFunctionalNode>(level, gui::dom::DomFunctionalNode::Type::NodeEnd));
}

void GuiDOMVisitor::visit(gui::Item &item)
{
    handler->onNode(gui::names::item);
    handler->handle(item);
    handler->onNodeEnd();
}

void GuiDOMVisitor::visit(gui::Rect &item)
{
    handler->onNode(gui::names::rect);
    handler->handle(item);
    handler->onNodeEnd();
}

void GuiDOMVisitor::visit(gui::Text &item)
{
    handler->onNode(gui::names::text);
    handler->handle(item);
    handler->onNodeEnd();
}

void GuiDOMVisitor::visit(gui::Window &item)
{
    visit(static_cast<Item &>(item));
}

void GuiDOMVisitor::visit(gui::Label &item)
{
    handler->onNode(gui::names::label);
    handler->handle(item);
    handler->onNodeEnd();
}

void GuiDOMVisitor::visit(gui::BottomBar &item)
{
    handler->onNode(gui::names::bottomBar);
    handler->handle(item);
    handler->onNodeEnd();
}

void GuiDOMVisitor::visit(gui::TopBar &item)
{
    handler->onNode(gui::names::topBar);
    handler->handle(item);
    handler->onNodeEnd();
}

void GuiDOMVisitor::dumpDOM()
{
    std::list<std::string> document;
    for (auto &node : handler->get()) {
        node->visit(document);
    }
    for (auto &line : document) {
        std::cout << line << std::endl;
    }
}

GuiDOMVisitor::GuiDOMVisitor()
{
    handler = new DOMDataHandler(*this);
}
GuiDOMVisitor::~GuiDOMVisitor()
{
    if (handler != nullptr) {
        delete handler;
    }
}
