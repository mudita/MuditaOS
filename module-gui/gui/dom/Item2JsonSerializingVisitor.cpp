// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Item2JsonSerializingVisitor.hpp"
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
    constexpr inline auto childrenName = "XChildren";
}

void Item2JsonSerializingVisitor::visit(gui::Item &item)
{
    auto boundingBoxSerializer = [](gui::BoundingBox &box) { return json11::Json::array{box.x, box.y, box.w, box.h}; };

    sink.emplace(names::item,
                 json11::Json::object{{item::itemType, static_cast<int>(item.type)},
                                      {item::itemType, static_cast<int>(item.type)},
                                      {item::focus, item.focus},
                                      {item::visible, item.visible},
                                      {item::active, item.activeItem},
                                      {item::childrenCount, static_cast<int>(item.children.size())},
                                      {item::widgetArea, boundingBoxSerializer(item.widgetArea)},
                                      {item::widgetMinimumArea, boundingBoxSerializer(item.widgetMinimumArea)},
                                      {item::widgetMaximumArea, boundingBoxSerializer(item.widgetMaximumArea)},
                                      {item::drawArea, boundingBoxSerializer(item.drawArea)}});
}

void Item2JsonSerializingVisitor::visit(gui::Rect &item)
{
    auto colorSerializer = [](gui::Color &color) {
        return json11::Json::array{static_cast<int>(color.intensity), static_cast<int>(color.alpha)};
    };
    sink.emplace(names::rect,
                 json11::Json::object{{rect::borderColor, colorSerializer(item.borderColor)},
                                      {rect::fillColor, colorSerializer(item.fillColor)},
                                      {rect::penWidth, item.penWidth},
                                      {rect::penFocusWidth, item.penFocusWidth},
                                      {rect::filled, item.filled},
                                      {rect::edges, static_cast<int>(item.edges)},
                                      {rect::flatEdges, static_cast<int>(item.flatEdges)},
                                      {rect::corners, static_cast<int>(item.corners)},
                                      {rect::yaps, static_cast<int>(item.yaps)},
                                      {rect::yapSize, item.yapSize}});
    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Text &item)
{
    sink.emplace(names::text, json11::Json::object{});
    visit(static_cast<gui::Rect &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Label &item)
{
    sink.emplace(names::label, json11::Json::object{});
    visit(static_cast<gui::Rect &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Window &item)
{

    sink.emplace(names::window, json11::Json::object{});
    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::BottomBar &item)
{
    visit(static_cast<gui::Item &>(item));
    sink.emplace(names::bottomBar, json11::Json::object{});
}

void Item2JsonSerializingVisitor::visit(gui::TopBar &item)
{
    sink.emplace(names::topBar, json11::Json::object{});
    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::handleSibling(gui::ItemNode &node, document &doc, int &level)
{
    node.getItem().accept(*this);
    doc[level].emplace_back(std::move(sink));
}

void Item2JsonSerializingVisitor::handleParent(gui::ItemNode &node, document &doc, int &level)
{
    auto parentLevel = node.getLevel();
    node.getItem().accept(*this);
    doc[parentLevel].emplace_back(std::move(sink));

    if (!doc[level].empty()) {
        auto children = json11::Json::array{};

        while (!doc[level].empty()) {
            auto &&obj = doc[level].front();
            children.emplace_back(std::move(obj));
            doc[level].pop_front();
        }

        auto &parent = doc[parentLevel].back();
        parent.emplace(childrenName, std::move(children));
    }

    doc[level].clear();
    level = parentLevel;
}

void Item2JsonSerializingVisitor::handleOther(gui::ItemNode &node, document &doc, int &level)
{
    level      = node.getLevel();
    doc[level] = std::list<json11::Json::object>{};

    node.getItem().accept(*this);
    doc[level].emplace_back(std::move(sink));
}

void Item2JsonSerializingVisitor::traverse(gui::Item &root)
{
    int level = 0;
    document documentPrototype;
    DepthFirstItemTree nodes(root, DepthFirstItemTree::TraverseMode::PostOrder);

    while (nodes.hasNode()) {
        auto node = nodes.getNext();
        if (node.getLevel() == level) {
            handleSibling(node, documentPrototype, level);
        }
        else if (node.getLevel() < level) {
            handleParent(node, documentPrototype, level);
        }
        else {
            handleOther(node, documentPrototype, level);
        }
    }

    json11::Json document(std::move(documentPrototype[0].front()));
    documentPrototype[0].clear();
    documentPrototype.clear();
    std::cout << document.dump() << std::endl;
}
