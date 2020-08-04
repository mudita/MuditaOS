/*
 * Item.cpp
 *
 *  Created on: 4 mar 2019
 *      Author: robert
 */

#include <list>
#include <algorithm>
#include <cstring>
#include "Navigation.hpp"
#include "Item.hpp"
#include "BoundingBox.hpp"
#include "log/log.hpp"

namespace gui
{

    NavigationDirection inputToNavigation(const InputEvent &evt)
    {
        switch (evt.keyCode) {
        case gui::KeyCode::KEY_LEFT:
            return gui::NavigationDirection::LEFT;
        case gui::KeyCode::KEY_RIGHT:
            return gui::NavigationDirection::RIGHT;
        case gui::KeyCode::KEY_UP:
            return gui::NavigationDirection::UP;
        case gui::KeyCode::KEY_DOWN:
            return gui::NavigationDirection::DOWN;
        default:
            return gui::NavigationDirection::NONE;
        }
    }

    Item::Item()
        : focus{false}, type{ItemType::ITEM}, parent{nullptr}, radius{0}, visible{true},
          verticalPolicy{LayoutVerticalPolicy::LAYOUT_POLICY_VERTICAL_EXPAND},
          horizontalPolicy{LayoutHorizontalPolicy::LAYOUT_POLICY_HORIZONTAL_EXPAND}, navigationDirections{nullptr}
    {

        focusChangedCallback     = [](Item &) { return false; };
        activatedCallback        = [](Item &) { return false; };
        inputCallback            = [](Item &, const InputEvent &inputEvent) { return false; };
        dimensionChangedCallback = [](Item &, void *data) { return false; };
        contentCallback          = [=](Item &) {
            if (parent != nullptr)
                return parent->onContent();
            return false;
        };
    }

    Item::~Item()
    {
        for (auto &widget : children)
            delete widget;

        if (navigationDirections)
            delete navigationDirections;
    }

    bool Item::erase(Item *item)
    {
        if (removeWidget(item)) {
            delete item;
            return true;
        }
        return false;
    }

    void Item::erase()
    {
        while (auto el = children.back()) {
            erase(el);
        }
    }

    void Item::addWidget(Item *item)
    {
        if (item == nullptr)
            return;
        if (item->parent) {
            item->parent->removeWidget(item);
        }
        item->parent = this;
        children.push_back(item);

        item->updateDrawArea();
    }

    bool Item::removeWidget(Item *item)
    {
        if (item == nullptr)
            return false;
        if (item == focusItem) {
            focusItem = nullptr;
        }

        auto fi = std::find(children.begin(), children.end(), item);
        if (fi != children.end()) {
            children.erase(fi);
            item->parent = nullptr;
            return true;
        }
        return false;
    }

    void Item::setVisible(bool value)
    {
        visible = value;
    }

    std::list<DrawCommand *> Item::buildDrawList()
    {

        std::list<DrawCommand *> commands;

        for (auto &widget : children) {
            std::list<DrawCommand *> widgetCommands = widget->buildDrawList();
            if (!widgetCommands.empty())
                commands.insert(commands.end(), widgetCommands.begin(), widgetCommands.end());
        }

        return commands;
    }

    void Item::setArea(BoundingBox area)
    {
        BoundingBox oldArea = widgetArea;
        widgetArea          = area;
        widgetMaximumArea.sum(widgetArea);
        updateDrawArea();
        onDimensionChanged(oldArea, widgetArea);
    }

    void Item::setPosition(const short &x, const short &y)
    {
        setArea({x, y, widgetArea.w, widgetArea.h});
    }

    void Item::setX(const int32_t x)
    {
        setArea({x, widgetArea.y, widgetArea.w, widgetArea.h});
    }

    void Item::setY(const int32_t y)
    {
        setArea({widgetArea.x, y, widgetArea.w, widgetArea.h});
    }

    auto Item::requestSize(unsigned short request_w, unsigned short request_h) -> Size
    {
        if (parent == nullptr) {
            setSize(request_w, request_h);
            return {request_w, request_h};
        }
        return parent->handleRequestResize(this, request_w, request_h);
    }

    auto Item::handleRequestResize(const Item *it, unsigned short request_w, unsigned short request_h) -> Size
    {
        if (it == nullptr) {
            return {0, 0};
        }
        return {it->getWidth(), it->getHeight()};
    }

    void Item::setSize(const unsigned short w, const unsigned short h)
    {
        setArea({widgetArea.x, widgetArea.y, w, h});
    }

    void Item::setSize(uint32_t val, Axis axis)
    {
        uint16_t w = getWidth();
        uint16_t h = getHeight();
        if (axis == Axis::X) {
            w = val;
        }
        else {
            h = val;
        }
        setArea({getX(), getY(), w, h});
    }

    void setAreaSizeInAxis(BoundingBox &area, Axis axis, uint32_t size)
    {
        if (axis == Axis::X) {
            area.w = size;
        }
        else {
            area.h = size;
        }
    }

    void Item::setAreaInAxis(
        Axis axis, uint32_t posOnAxis, uint32_t posOnOrthogonalAxis, uint32_t sizeOnAxis, uint32_t sizeOnOrthogonalAxis)
    {
        if (axis == Axis::X) {

            auto box = BoundingBox(posOnAxis, posOnOrthogonalAxis, sizeOnAxis, sizeOnOrthogonalAxis);

            if (widgetArea != box)
                setArea(box);
        }
        else {

            auto box = BoundingBox(posOnOrthogonalAxis, posOnAxis, sizeOnOrthogonalAxis, sizeOnAxis);

            if (widgetArea != box)
                setArea(box);
        }
    }

    /// set maximum
    void Item::setMaximumSize(uint32_t val, Axis axis)
    {
        setAreaSizeInAxis(area(Area::Max), axis, val);
    }

    void Item::setMaximumWidth(uint32_t w)
    {
        setMaximumSize(w, Axis::X);
    }

    void Item::setMaximumHeight(uint32_t h)
    {
        setMaximumSize(h, Axis::Y);
    }

    void Item::setMaximumSize(uint32_t w, uint32_t h)
    {
        setMaximumHeight(h);
        setMaximumWidth(w);
    }

    /// set minimum

    void Item::setMinimumSize(uint32_t val, Axis axis)
    {
        setAreaSizeInAxis(area(Area::Min), axis, val);
    }

    void Item::setMinimumWidth(uint32_t w)
    {
        setMinimumSize(w, Axis::X);
    }

    void Item::setMinimumHeight(uint32_t h)
    {
        setMinimumSize(h, Axis::Y);
    }

    void Item::setMinimumSize(uint32_t w, uint32_t h)
    {
        setMinimumWidth(w);
        setMinimumHeight(h);
    }

    /// set and get Margins
    void Item::setMargins(const Margins &value)
    {
        if (margins != value) {
            margins = value;
        }
    }

    Margins Item::getMargins()
    {
        return margins;
    }

    void Item::setPadding(const Padding &value)
    {
        if (padding != value) {
            padding = value;
        }
    }

    Padding Item::getPadding()
    {
        return padding;
    }

    uint16_t Item::getSize(Axis axis) const
    {
        return widgetArea.size(axis);
    }

    uint16_t Item::getPosition(Axis axis) const
    {
        return widgetArea.pos(axis);
    }

    void Item::setPosition(const short &val, Axis axis)
    {
        int16_t x = getX();
        int16_t y = getY();
        if (axis == Axis::X) {
            x = val;
        }
        else {
            y = val;
        }
        setArea({x, y, getWidth(), getHeight()});
    }

    void Item::setAlignment(const Alignment &value)
    {
        if (alignment != value) {
            alignment = value;
        }
    }

    Alignment Item::getAlignment(Axis axis)
    {
        if (axis == Axis::X) {
            return Alignment(alignment.horizontal, Alignment::Vertical::None);
        }
        else {
            return Alignment(Alignment::Horizontal::None, alignment.vertical);
        }
    }

    Alignment &Item::getAlignment()
    {
        return alignment;
    }

    uint16_t Item::getAxisAlignmentValue(Axis axis, uint16_t itemSize)
    {
        auto tempAlignment = getAlignment(axis);

        if (parent->getAlignment(axis).vertical != Alignment::Vertical::None)
            tempAlignment.vertical = parent->getAlignment(axis).vertical;

        if (parent->getAlignment(axis).horizontal != Alignment::Horizontal::None)
            tempAlignment.horizontal = parent->getAlignment(axis).horizontal;

        switch (tempAlignment.vertical) {
        case gui::Alignment::Vertical::Top:
            return this->margins.getMarginInAxis(axis, MarginInAxis::First);
        case gui::Alignment::Vertical::Center:
            return (parent->area().size(axis) - (itemSize + this->margins.getSumInAxis(axis))) / 2;
        case gui::Alignment::Vertical::Bottom:
            return parent->area().size(axis) - itemSize - this->margins.getMarginInAxis(axis, MarginInAxis::Second);
        default:
            break;
        }

        switch (tempAlignment.horizontal) {
        case gui::Alignment::Horizontal::Left:
            return this->margins.getMarginInAxis(axis, MarginInAxis::First);
        case gui::Alignment::Horizontal::Center:
            return (parent->area().size(axis) - (itemSize + this->margins.getSumInAxis(axis))) / 2;
        case gui::Alignment::Horizontal::Right:
            return parent->area().size(axis) - itemSize - this->margins.getMarginInAxis(axis, MarginInAxis::Second);
        default:
            break;
        }

        return getPosition(axis);
    }

    void Item::setBoundingBox(const BoundingBox &new_box)
    {
        BoundingBox oldArea = widgetArea;
        widgetArea          = new_box;
        updateDrawArea();
        onDimensionChanged(oldArea, widgetArea);
    }

    void Item::setRadius(int value)
    {
        if (value < 0)
            value = 0;
        radius = value;
    }

    void Item::updateDrawArea()
    {
        Item *parentItem = parent;

        BoundingBox result = widgetArea;

        // iterate up to top widget in hierarchy
        while (parentItem != nullptr) {

            result.x += parentItem->widgetArea.x;
            result.y += parentItem->widgetArea.y;
            BoundingBox newResult;
            if (BoundingBox::intersect(parentItem->widgetArea, result, newResult) == false) {
                result.clear();
                break;
            }
            result = newResult;

            parentItem = parentItem->parent;
        }

        drawArea = result;

        for (gui::Item *it : children)
            it->updateDrawArea();
    }

    Item *Item::getNavigationItem(NavigationDirection direction)
    {
        if (navigationDirections != nullptr) {
            return navigationDirections->getDirectionItem(direction);
        }
        return nullptr;
    }

    void Item::setNavigationItem(gui::NavigationDirection direction, Item *item)
    {
        if (navigationDirections == nullptr)
            navigationDirections = new Navigation();
        navigationDirections->setDirectionItem(direction, item);
    }

    void Item::clearNavigationItem(gui::NavigationDirection direction)
    {
        if (navigationDirections != nullptr) {
            navigationDirections->clearDirection(direction);
        }
    }

    bool Item::handleNavigation(const InputEvent inputEvent)
    {
        gui::Item *newFocusItem = nullptr;
        if ((focusItem != nullptr) && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
            if (itemNavigation != nullptr && itemNavigation(inputEvent)) {
                return true;
            }
            if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER && focusItem != nullptr) {
                return focusItem->onActivated(nullptr);
            }
            newFocusItem = focusItem->getNavigationItem(inputToNavigation(inputEvent));
        }
        if (newFocusItem != nullptr) {
            setFocusItem(newFocusItem);
            return true;
        }
        return false;
    }

    bool Item::setFocus(bool state)
    {
        if (state != focus) {
            focus = state;
            onFocus(state);
            if (focusChangedCallback)
                focusChangedCallback(*this);
        };
        return state;
    }

    void Item::setFocusItem(Item *item)
    {
        auto checknrun = [=](bool on) {
            if (focusItem != nullptr) {
                focusItem->setFocus(on);
            }
        };
        checknrun(false);
        focusItem = item;
        checknrun(true);
    }

    Item *Item::getFocusItem()
    {
        if (focusItem) {
            auto subFocusItem = focusItem->getFocusItem();
            return subFocusItem == nullptr ? focusItem : subFocusItem;
        }
        return nullptr;
    }

    bool Item::onFocus(bool state)
    {
        focus = state;
        return true;
    }

    bool Item::onActivated(void *data)
    {
        if (activatedCallback)
            return activatedCallback(*this);
        return false;
    }

    bool Item::onInput(const InputEvent &inputEvent)
    {
        if (inputCallback) {
            return inputCallback(*this, inputEvent);
        }
        return false;
    }

    bool Item::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        if (dimensionChangedCallback) {
            return dimensionChangedCallback(*this, nullptr);
        }
        return false;
    }

    bool Item::onContent()
    {
        return false;
    }

} /* namespace gui */
