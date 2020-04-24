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

namespace gui
{

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

    void Item::setPosition(const short &x, const short &y)
    {

        BoundingBox oldArea = widgetArea;
        widgetArea.x        = x;
        widgetArea.y        = y;
        updateDrawArea();

        onDimensionChanged(oldArea, widgetArea);
    }

    void Item::setX(const uint32_t x)
    {
        BoundingBox oldArea = widgetArea;
        widgetArea.x        = x;
        updateDrawArea();

        onDimensionChanged(oldArea, widgetArea);
    }

    void Item::setY(const uint32_t y)
    {
        BoundingBox oldArea = widgetArea;
        widgetArea.y        = y;
        updateDrawArea();

        onDimensionChanged(oldArea, widgetArea);
    }

    void Item::setSize(const unsigned short w, const unsigned short h)
    {
        BoundingBox oldArea = widgetArea;
        widgetArea.w        = w;
        widgetArea.h        = h;
        widgetMaxArea.sum(widgetArea);
        updateDrawArea();

        onDimensionChanged(oldArea, widgetArea);
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

    bool Item::handleNavigation(const InputEvent inputEvent)
    {
        gui::Item *newFocusItem = nullptr;
        if ((focusItem != nullptr) && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
            if (itemNavigation && itemNavigation(inputEvent)) {
                return true;
            }
            switch (inputEvent.keyCode) {
            case gui::KeyCode::KEY_LEFT:
                newFocusItem = focusItem->getNavigationItem(gui::NavigationDirection::LEFT);
                break;
            case gui::KeyCode::KEY_RIGHT:
                newFocusItem = focusItem->getNavigationItem(gui::NavigationDirection::RIGHT);
                break;
            case gui::KeyCode::KEY_UP:
                newFocusItem = focusItem->getNavigationItem(gui::NavigationDirection::UP);
                break;
            case gui::KeyCode::KEY_DOWN:
                newFocusItem = focusItem->getNavigationItem(gui::NavigationDirection::DOWN);
                break;
            case gui::KeyCode::KEY_ENTER:
                if (focusItem != nullptr)
                    return focusItem->onActivated(nullptr);
                break;
            default:
                break;
            }
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
        return focusItem;
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
        return true;
    }

    bool Item::onContent()
    {
        return false;
    };
} /* namespace gui */
