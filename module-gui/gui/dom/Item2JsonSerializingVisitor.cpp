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

using namespace gui;
using namespace gui::visitor;


void Item2JsonSerializingVisitor::visit(gui::Item &item)
{
    auto boundingBoxSerializer = [](gui::BoundingBox &box) { return json11::Json::array{box.x, box.y, box.w, box.h}; };

    if (itemName.empty()) {
        itemName = names::item;
    }
    sink.emplace_back(json11::Json::object{{item::itemType, static_cast<int>(item.type)}});
    sink.emplace_back(json11::Json::object{{item::focus, item.focus}});
    sink.emplace_back(json11::Json::object{{item::visible, item.visible}});
    sink.emplace_back(json11::Json::object{{item::active, item.activeItem}});
    sink.emplace_back(json11::Json::object{{item::childrenCount, static_cast<int>(item.children.size())}});
    sink.emplace_back(json11::Json::object{{item::widgetArea, boundingBoxSerializer(item.widgetArea)}});
    sink.emplace_back(json11::Json::object{{item::widgetMinimumArea, boundingBoxSerializer(item.widgetMinimumArea)}});
    sink.emplace_back(json11::Json::object{{item::widgetMaximumArea, boundingBoxSerializer(item.widgetMaximumArea)}});
    sink.emplace_back(json11::Json::object{{item::drawArea, boundingBoxSerializer(item.drawArea)}});
}

void Item2JsonSerializingVisitor::visit(gui::Rect &item)
{
    auto colorSerializer = [](gui::Color &color) {
        return json11::Json::array{static_cast<int>(color.intensity), static_cast<int>(color.alpha)};
    };

    if (itemName.empty()) {
        itemName = names::rect;
    }
    sink.emplace_back(json11::Json::object{{rect::borderColor, colorSerializer(item.borderColor)}});
    sink.emplace_back(json11::Json::object{{rect::fillColor, colorSerializer(item.fillColor)}});
    sink.emplace_back(json11::Json::object{{rect::penWidth, item.penWidth}});
    sink.emplace_back(json11::Json::object{{rect::penFocusWidth, item.penFocusWidth}});
    sink.emplace_back(json11::Json::object{{rect::filled, item.filled}});
    sink.emplace_back(json11::Json::object{{rect::edges, static_cast<int>(item.edges)}});
    sink.emplace_back(json11::Json::object{{rect::flatEdges, static_cast<int>(item.flatEdges)}});
    sink.emplace_back(json11::Json::object{{rect::corners, static_cast<int>(item.corners)}});
    sink.emplace_back(json11::Json::object{{rect::yaps, static_cast<int>(item.yaps)}});
    sink.emplace_back(json11::Json::object{{rect::yapSize, item.yapSize}});

    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Text &item)
{
    if (itemName.empty()) {
        itemName = names::text;
    }
    sink.emplace_back(json11::Json::object{{visitor::text::text, std::string{item.getText()}}});
    visit(static_cast<gui::Rect &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Label &item)
{
    if (itemName.empty()) {
        itemName = names::label;
    }
    sink.emplace_back(json11::Json::object{{visitor::text::text, std::string{item.getText()}}});
    visit(static_cast<gui::Rect &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Window &item)
{
    if (itemName.empty()) {
        itemName = names::window;
    }
    sink.emplace_back(json11::Json::object{{window::name, std::string{item.getName()}}});
    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::BottomBar &item)
{
    if (itemName.empty()) {
        itemName = names::bottomBar;
    }
    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::TopBar &item)
{
    if (itemName.empty()) {
        itemName = names::topBar;
    }
    visit(static_cast<gui::Item &>(item));
}
