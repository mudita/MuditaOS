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

        resizeItems();
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

    // space left distposition `first is better` tactics
    // there could be other i.e. socialism: each element take in equal part up to it's max size
    // not needed now == not implemented
    template <Axis axis> void BoxLayout::resizeItems()
    {
        // we want to split to interested parties what's left, as much as they can fit
        auto to_split   = sizeLeft<axis>(this);
        auto pos        = reverse_order ? this->area().size(axis) : 0;
        auto pos_update = [this, &pos](Item *it) {
            if (this->reverse_order) {
                pos -= it->area(Item::Area::Actual).size(axis);
                it->area(Item::Area::Actual).pos(axis) = pos;
                it->area(Item::Area::Normal).pos(axis) = pos;
            }
            else {
                it->area(Item::Area::Actual).pos(axis) = pos;
                it->area(Item::Area::Normal).pos(axis) = pos;
                pos += it->area(Item::Area::Actual).size(axis);
            }
        };

        auto set_size = [this, &to_split, &pos_update](Item *el, auto &pos) {
            if (el == nullptr) {
                return;
            }
            auto left_in_el = el->area(Item::Area::Max).size(axis) - el->area(Item::Area::Normal).size(axis);
            if (to_split > 0 && left_in_el > 0) {
                auto resize = left_in_el < to_split ? left_in_el : to_split;
                el->area(Item::Area::Actual).size(axis) += resize;
                to_split -= resize;
            }
        };

        for (auto &el : children) {
            el->area(Item::Area::Actual) = el->area(Item::Area::Normal);
            set_size(el, pos);
            pos_update(el);
            // LOG_DEBUG("> el %s", el->area(Area::Normal).str().c_str()); // log to show how inefficient is adding
            // single element at a time
        }
        Rect::updateDrawArea();
    }

    template <Axis axis> void BoxLayout::addWidget(Item *item)
    {
        Rect::addWidget(item);
        item->visible = false;
        if (size<axis>(this) - sizeUsed<axis>(this) >= size<axis>(item)) {
            item->visible = true;
            resizeItems<axis>();
        }
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

    void BoxLayout::setNavigation()
    {
        auto previous = nextNavigationItem(children.begin()), next = children.end();
        int i = 0;
        if (type == ItemType::VBOX) {
            while ((previous != children.end()) &&
                   ((next = nextNavigationItem(std::next(previous))) != std::end(children))) {
                (*previous)->setNavigationItem(reverse_order ? NavigationDirection::UP : NavigationDirection::DOWN,
                                               *next);
                (*next)->setNavigationItem(reverse_order ? NavigationDirection::DOWN : NavigationDirection::UP,
                                           *previous);
                previous = next;
            }
        }

        if (type == ItemType::HBOX) {
            while ((previous != children.end()) &&
                   ((next = nextNavigationItem(std::next(previous))) != std::end(children))) {
                (*previous)->setNavigationItem(reverse_order ? NavigationDirection::LEFT : NavigationDirection::RIGHT,
                                               *next);
                (*next)->setNavigationItem(reverse_order ? NavigationDirection::RIGHT : NavigationDirection::LEFT,
                                           *previous);
                previous = next;
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

} /* namespace gui */
