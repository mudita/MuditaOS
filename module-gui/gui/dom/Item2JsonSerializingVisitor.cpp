// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Item2JsonSerializingVisitor.hpp"
#include "Item.hpp"
#include "Rect.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include "BottomBar.hpp"
#include "StatusBar.hpp"
#include "ListItem.hpp"

#include "ItemDataNames.hpp"
#include <magic_enum.hpp>

using namespace gui;

void Item2JsonSerializingVisitor::visit(gui::Item &item)
{
    if (itemName.empty()) {
        itemName = magic_enum::enum_name(visitor::Names::Item);
    }
    sink.emplace(magic_enum::enum_name(visitor::Item::ItemType), static_cast<int>(item.type));
    sink.emplace(magic_enum::enum_name(visitor::Item::Focus), item.focus);
    sink.emplace(magic_enum::enum_name(visitor::Item::Visible), item.visible);
    sink.emplace(magic_enum::enum_name(visitor::Item::Active), item.activeItem);
    sink.emplace(magic_enum::enum_name(visitor::Item::ChildrenCount), static_cast<int>(item.children.size()));
    sink.emplace(magic_enum::enum_name(visitor::Item::WidgetArea), serialize(item.widgetArea));
    sink.emplace(magic_enum::enum_name(visitor::Item::WidgetMinimumArea), serialize(item.widgetMinimumArea));
    sink.emplace(magic_enum::enum_name(visitor::Item::WidgetMaximumArea), serialize(item.widgetMaximumArea));
    sink.emplace(magic_enum::enum_name(visitor::Item::DrawArea), serialize(item.drawArea));
}

void Item2JsonSerializingVisitor::visit(gui::Rect &item)
{
    if (itemName.empty()) {
        itemName = magic_enum::enum_name(visitor::Names::Rect);
    }
    sink.emplace(magic_enum::enum_name(visitor::Rect::BorderColor), serialize(item.borderColor));
    sink.emplace(magic_enum::enum_name(visitor::Rect::FillColor), serialize(item.fillColor));
    sink.emplace(magic_enum::enum_name(visitor::Rect::PenWidth), item.penWidth);
    sink.emplace(magic_enum::enum_name(visitor::Rect::PenFocusWidth), item.penFocusWidth);
    sink.emplace(magic_enum::enum_name(visitor::Rect::Filled), item.filled);
    sink.emplace(magic_enum::enum_name(visitor::Rect::Edges), static_cast<int>(item.edges));
    sink.emplace(magic_enum::enum_name(visitor::Rect::FlatEdges), static_cast<int>(item.flatEdges));
    sink.emplace(magic_enum::enum_name(visitor::Rect::Corners), static_cast<int>(item.corners));
    sink.emplace(magic_enum::enum_name(visitor::Rect::Yaps), static_cast<int>(item.yaps));
    sink.emplace(magic_enum::enum_name(visitor::Rect::YapSize), item.yapSize);

    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Text &item)
{
    if (itemName.empty()) {
        itemName = magic_enum::enum_name(visitor::Names::Text);
    }
    sink.emplace(magic_enum::enum_name(visitor::Text::TextValue), std::string{item.getText()});
    visit(static_cast<gui::Rect &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Label &item)
{
    if (itemName.empty()) {
        itemName = magic_enum::enum_name(visitor::Names::Label);
    }
    sink.emplace(magic_enum::enum_name(visitor::Text::TextValue), std::string{item.getText()});
    visit(static_cast<gui::Rect &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::Window &item)
{
    if (itemName.empty()) {
        itemName = magic_enum::enum_name(visitor::Names::Window);
    }
    sink.emplace(magic_enum::enum_name(visitor::Window::WindowName), std::string{item.getUniqueName()});
    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::BottomBar &item)
{
    if (itemName.empty()) {
        itemName = magic_enum::enum_name(visitor::Names::BottomBar);
    }
    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::status_bar::StatusBar &item)
{
    if (itemName.empty()) {
        itemName = magic_enum::enum_name(visitor::Names::StatusBar);
    }
    visit(static_cast<gui::Item &>(item));
}

void Item2JsonSerializingVisitor::visit(gui::ListItem &item)
{
    if (itemName.empty()) {
        itemName = magic_enum::enum_name(visitor::Names::ListItem);
    }
    visit(static_cast<gui::Rect &>(item));
}

auto Item2JsonSerializingVisitor::serialize(gui::BoundingBox &box) -> json11::Json::array
{
    return {static_cast<int>(box.x), static_cast<int>(box.y), static_cast<int>(box.w), static_cast<int>(box.h)};
}

auto Item2JsonSerializingVisitor::serialize(gui::Color &color) -> json11::Json::array
{
    return {static_cast<int>(color.intensity), static_cast<int>(color.alpha)};
}
