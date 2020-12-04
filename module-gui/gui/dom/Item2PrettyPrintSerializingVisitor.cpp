// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Item2PrettyPrintSerializingVisitor.hpp"

#include "Item.hpp"
#include "Rect.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include "BottomBar.hpp"
#include "TopBar.hpp"

#include "ItemDataNames.hpp"
#include "DepthFirstItemTree.hpp"

using namespace gui;
using namespace gui::visitor;

namespace
{
    constexpr inline auto nodeBegin = "NextItem{";
    constexpr inline auto nodeEnd   = "}";
    constexpr inline auto span      = "\t";

    constexpr inline auto delimiter      = " : ";
    constexpr inline auto arrayBegin     = "[";
    constexpr inline auto arrayEnd       = "]";
    constexpr inline auto arrayDelimiter = ", ";
} // namespace

void Item2PrettyPrintSerializingVisitor::visit(gui::Item &item)
{
    auto boundingBox2Vec = [](gui::BoundingBox &box) -> std::vector<int> { return {box.x, box.y, box.w, box.h}; };

    addLine(item::itemType, static_cast<int>(item.type)); // TODO: tostring>?
    addLine(item::focus, item.focus);
    addLine(item::visible, item.visible);
    addLine(item::active, item.activeItem);
    addLine(item::childrenCount, static_cast<int>(item.children.size()));
    addLine(item::widgetArea, boundingBox2Vec(item.widgetArea));
    addLine(item::widgetMinimumArea, boundingBox2Vec(item.widgetMinimumArea));
    addLine(item::widgetMaximumArea, boundingBox2Vec(item.widgetMaximumArea));
    addLine(item::drawArea, boundingBox2Vec(item.drawArea));
}

void Item2PrettyPrintSerializingVisitor::visit(gui::Rect &item)
{
    auto colorVec = [](gui::Color &color) {
        return std::vector<int>{static_cast<int>(color.intensity), static_cast<int>(color.alpha)};
    };

    addLine(rect::borderColor, colorVec(item.borderColor));
    addLine(rect::fillColor, colorVec(item.fillColor));
    addLine(rect::penWidth, item.penWidth);
    addLine(rect::penFocusWidth, item.penFocusWidth);
    addLine(rect::filled, item.filled);
    addLine(rect::edges, static_cast<int>(item.edges));
    addLine(rect::flatEdges, static_cast<int>(item.flatEdges));
    addLine(rect::corners, static_cast<int>(item.corners));
    addLine(rect::yaps, static_cast<int>(item.yaps));
    addLine(rect::yapSize, item.yapSize);

    visit(static_cast<gui::Item &>(item));
}

void Item2PrettyPrintSerializingVisitor::visit(gui::Text &item)
{

    visit(static_cast<gui::Rect &>(item));
}

void Item2PrettyPrintSerializingVisitor::visit(gui::Label &item)
{

    visit(static_cast<gui::Rect &>(item));
}

void Item2PrettyPrintSerializingVisitor::visit(gui::Window &item)
{

    visit(static_cast<gui::Item &>(item));
}

void Item2PrettyPrintSerializingVisitor::visit(gui::BottomBar &item)
{

    visit(static_cast<gui::Item &>(item));
}

void Item2PrettyPrintSerializingVisitor::visit(gui::TopBar &item)
{

    visit(static_cast<gui::Item &>(item));
}

void Item2PrettyPrintSerializingVisitor::addLine(const std::string &name, const std::string &value)
{
    sink.emplace_back(name + delimiter + value);
}
void Item2PrettyPrintSerializingVisitor::addLine(const std::string &name, const std::vector<int> &value)
{
    std::string sValue = arrayBegin;
    for (auto val : value) {
        sValue += std::to_string(val) + arrayDelimiter;
    }
    sValue += arrayEnd;
    addLine(name, sValue);
}
void Item2PrettyPrintSerializingVisitor::addLine(const std::string &name, int value)
{
    addLine(name, std::to_string(value));
}
void Item2PrettyPrintSerializingVisitor::addLine(const std::string &name, bool value)
{
    addLine(name, std::to_string(static_cast<int>(value)));
}

void Item2PrettyPrintSerializingVisitor::traverse(gui::Item &root)
{
    std::list<std::string> documentPrototype;
    DepthFirstItemTree nodes(root, DepthFirstItemTree::TraverseMode::PreOrder);
    while (nodes.hasNode()) {
        auto node = nodes.getNext();

        sink.emplace_back(nodeBegin);
        node.getItem().accept(*this);
        sink.emplace_back(nodeEnd);

        std::string spans;
        for (int i = 0; i < node.getLevel(); i++) {
            spans += span;
        }

        for (auto &line : sink) {
            line = spans + line;
        }

        documentPrototype.splice(documentPrototype.end(), std::move(sink));
    }

    for (auto line : documentPrototype) {
        std::cout << line << std::endl;
    }
}
