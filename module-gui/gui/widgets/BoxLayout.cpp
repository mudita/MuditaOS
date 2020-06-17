/*
 * BoxLayout.cpp
 *
 *  Created on: 16 maj 2019
 *      Author: robert
 */

#include "BoxLayout.hpp"
#include <log/log.hpp>

#include <Label.hpp>

namespace gui
{

    BoxLayout::BoxLayout() : BoxLayout(nullptr, 0, 0, 0, 0)
    {}

    BoxLayout::BoxLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
        : Rect(parent, x, y, w, h)
    {}

    bool BoxLayout::onInput(const InputEvent &inputEvent)
    {
        if (inputCallback && inputCallback(*this, inputEvent)) {
            return true;
        }
        if (focusItem && focusItem->onInput(inputEvent)) {
            return true;
        }
        if (handleNavigation(inputEvent)) {
            return true;
        }
        if (borderCallback && borderCallback(inputEvent)) {
            outOfDrawAreaItems.clear();
            return true;
        }
        // let item logic rule it
        return false;
    }

    bool BoxLayout::onFocus(bool state)
    {
        if (state)
            this->setVisible(state);
        else
            this->setFocusItem(nullptr);
        this->setNavigation();
        if (this->focusChangedCallback) {
            this->focusChangedCallback(*this);
        }
        return true;
    }

    void BoxLayout::resizeItems()
    {}

    void BoxLayout::setPosition(const short &x, const short &y)
    {
        Rect::setPosition(x, y);
    }

    void BoxLayout::setSize(const unsigned short w, const unsigned short h)
    {
        Rect::setSize(w, h);

        if (children.size() != 0u) {
            for (auto it : outOfDrawAreaItems) {
                it->setVisible(true);
            }
        }
        outOfDrawAreaItems.clear();

        resizeItems();
        setNavigation();
    }

    void BoxLayout::addWidget(Item *item)
    {
        Rect::addWidget(item);
        resizeItems();
    }

    bool BoxLayout::removeWidget(Item *item)
    {
        bool ret = Rect::removeWidget(item);

        resizeItems();

        return ret;
    }

    bool BoxLayout::erase(Item *item)
    {
        auto ret = Item::erase(item);

        outOfDrawAreaItems.clear();
        resizeItems();

        return ret;
    }

    void BoxLayout::erase()
    {
        Item::erase();
    }

    std::list<DrawCommand *> BoxLayout::buildDrawList()
    {
        auto el = Rect::buildDrawList();
        return el;
    }

    void BoxLayout::setVisible(bool value, bool previous)
    {
        visible = value; // maybe use parent setVisible(...)? would be better but which one?
        if (value == true) {
            resizeItems();         // move items in box in proper places
            setNavigation();       // set navigation through kids -> TODO handle out of last/first to parent
            if (children.size()) { // set first visible kid as focused item - TODO should check for actionItems too...
                /// this if back / front is crappy :|
                if (previous) {
                    for (auto el = children.rbegin(); el != children.rend(); ++el) {
                        if ((*el)->visible && (*el)->activeItem) {
                            setFocusItem(*el);
                            break;
                        }
                    }
                }
                else {
                    for (auto &el : children) {
                        if (el->visible && el->activeItem) {
                            setFocusItem(el);
                            break;
                        }
                    }
                }
            }
        }
    }

    void BoxLayout::setVisible(bool value)
    {
        setVisible(value, false);
    }

    void BoxLayout::setReverseOrder(bool value)
    {
        reverseOrder = value;
    }

    void BoxLayout::setAxisAlignment(bool value)
    {
        axisAlignment = value;
    }

    void BoxLayout::addToOutOfDrawAreaList(Item *it)
    {
        if (it->visible) {
            outOfDrawAreaItems.push_back(it);
            it->visible = false;
        }
    }

    // space left distposition `first is better` tactics
    // there could be other i.e. socialism: each element take in equal part up to it's max size
    // not needed now == not implemented
    template <Axis axis> void BoxLayout::resizeItems()
    {

        // we want to split to interested parties what's left, as much as they can fit
        int32_t pos      = reverseOrder ? this->area().size(axis) : 0;
        int32_t pos_left = this->getSize(axis);
        int32_t to_split = sizeLeft<axis>(this);

        auto set_size_and_pos = [this](Item *it, int32_t &to_split, int32_t &pos, int32_t &pos_left) {
            if (it == nullptr) {
                return;
            }

            auto axisItemPosition       = 0;
            auto orthogonalItemPosition = 0;
            auto axisItemSize           = 0;
            auto orthogonalItemSize     = 0;

            // Check if item can be resized
            int32_t left_in_el = it->area(Area::Max).size(axis) - it->area(Area::Min).size(axis);
            if (to_split > 0 && left_in_el > 0) {
                int32_t resize = std::min(left_in_el, to_split);
                axisItemSize   = it->area(Area::Min).size(axis) + resize;
                to_split -= resize;
            }
            else {
                axisItemSize = it->area(Area::Min).size(axis);
            }

            orthogonalItemSize =
                std::min(this->area(Area::Normal).size(orthogonal(axis)), it->area(Area::Max).size(orthogonal(axis)));

            // Check if there is still position left
            if (axisItemSize <= pos_left) {
                if (reverseOrder) {
                    pos -= axisItemSize;
                }

                axisItemPosition = pos;

                if (!reverseOrder) {
                    pos += axisItemSize;
                }

                pos_left -= axisItemSize;
            }
            else {
                addToOutOfDrawAreaList(it);
            }

            if (it->visible)
                it->setAreaInAxis(axis, axisItemPosition, orthogonalItemPosition, axisItemSize, orthogonalItemSize);
        };

        for (auto &el : children) {

            if (!el->visible)
                continue;

            set_size_and_pos(el, to_split, pos, pos_left);
        }

        Rect::updateDrawArea();
    }

    template <Axis axis> void BoxLayout::addWidget(Item *item)
    {
        outOfDrawAreaItems.clear();
        Rect::addWidget(item);
        resizeItems<axis>();
    }

    std::list<Item *>::iterator BoxLayout::nextNavigationItem(std::list<Item *>::iterator from)
    {
        return std::find_if(from, this->children.end(), [](auto &el) -> bool {
            if (el->visible && el->activeItem) {
                return true;
            }
            return false;
        });
    }

    template <Axis axis> void BoxLayout::reverseAlignment()
    {
        for (auto &it : children) {
            it->setPosition(it->getPosition(axis) - sizeLeft<axis>(this), axis);
        }
    }

    void BoxLayout::setNavigation()
    {
        auto previous = nextNavigationItem(children.begin()), next = children.end();
        if (type == ItemType::VBOX) {
            while ((previous != children.end()) &&
                   ((next = nextNavigationItem(std::next(previous))) != std::end(children))) {
                (*previous)->setNavigationItem(reverseOrder ? NavigationDirection::UP : NavigationDirection::DOWN,
                                               *next);
                (*next)->setNavigationItem(reverseOrder ? NavigationDirection::DOWN : NavigationDirection::UP,
                                           *previous);
                previous = next;
            }
        }

        if (type == ItemType::HBOX) {
            while ((previous != children.end()) &&
                   ((next = nextNavigationItem(std::next(previous))) != std::end(children))) {
                (*previous)->setNavigationItem(reverseOrder ? NavigationDirection::LEFT : NavigationDirection::RIGHT,
                                               *next);
                (*next)->setNavigationItem(reverseOrder ? NavigationDirection::RIGHT : NavigationDirection::LEFT,
                                           *previous);
                previous = next;
            }
        }
    }

    void BoxLayout::setFocusOnElement(uint32_t elementNumber)
    {
        uint32_t i = 0;
        for (auto child : children) {
            if (child->activeItem == true && child->visible == true) {

                if (elementNumber == i) {
                    child->setFocus(true);
                    focusItem = child;
                }
                else {
                    child->setFocus(false);
                }
                ++i;
            }
        }
    }

    void BoxLayout::setFocusOnLastElement()
    {
        auto last = true;
        for (auto child = children.rbegin(); child != children.rend(); child++) {

            if ((*child)->activeItem && (*child)->visible && last) {
                (*child)->setFocus(true);
                focusItem = (*child);
                last      = false;
            }
            else {
                (*child)->setFocus(false);
            }
        }
    }

    HBox::HBox() : BoxLayout()
    {
        type = ItemType::HBOX;
    }
    HBox::HBox(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
        : BoxLayout(parent, x, y, w, h)
    {
        type = ItemType::HBOX;
    }

    void HBox::resizeItems()
    {
        BoxLayout::resizeItems<Axis::X>();
    }

    void HBox::addWidget(Item *item)
    {
        BoxLayout::addWidget<Axis::X>(item);
    }

    void HBox::axisAlignment()
    {
        if (reverseOrder && BoxLayout::axisAlignment) {
            reverseAlignment<Axis::X>();
        }
    }

    VBox::VBox() : BoxLayout()
    {
        type = ItemType::VBOX;
    }
    VBox::VBox(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
        : BoxLayout(parent, x, y, w, h)
    {
        type = ItemType::VBOX;
    }

    void VBox::resizeItems()
    {
        BoxLayout::resizeItems<Axis::Y>();
    }

    void VBox::addWidget(Item *item)
    {
        BoxLayout::addWidget<Axis::Y>(item);
    }

    void VBox::axisAlignment()
    {
        if (reverseOrder && BoxLayout::axisAlignment) {
            reverseAlignment<Axis::Y>();
        }
    }

} /* namespace gui */
