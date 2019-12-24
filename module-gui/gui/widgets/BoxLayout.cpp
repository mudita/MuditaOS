/*
 * BoxLayout.cpp
 *
 *  Created on: 16 maj 2019
 *      Author: robert
 */

#include "BoxLayout.hpp"
#include <log/log.hpp>

#include <Label.hpp>

namespace gui {

    BoxLayout::BoxLayout() : BoxLayout(nullptr, 0, 0, 0, 0)
    {
    }

    BoxLayout::BoxLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h) : Rect(parent, x, y, w, h)
    {
        inputCallback = [this](Item &item, const InputEvent &inputEvent) -> bool {
            // handle input on kid
            if (focusItem && focusItem->onInput(inputEvent))
            {
                return true;
            }
            if (handleNavigation(inputEvent))
            {
                return true;
            }
            // let item logic rule it
            return false;
        };
    }

    bool BoxLayout::onFocus(bool state)
    {
        this->setVisible(state);
        this->setNavigation();
        if (this->focusChangedCallback)
        {
            this->focusChangedCallback(*this);
        }
        return true;
    }

void BoxLayout::resizeItems() {
}

void BoxLayout::setPosition( const short& x, const short& y ) {
	Rect::setPosition( x, y );
}
void BoxLayout::setSize( const short& w, const short& h ) {
	Rect::setSize( w, h );

	resizeItems();
}
bool BoxLayout::addWidget( Item* item ) {
	bool ret = Rect::addWidget( item );

	resizeItems();

	return ret;
}
bool BoxLayout::removeWidget( Item* item ) {
	bool ret = Rect::removeWidget( item );

	resizeItems();

	return ret;
}
std::list<DrawCommand*> BoxLayout::buildDrawList() {
	return Rect::buildDrawList();
}

void BoxLayout::setVisible(bool value, bool previous)
{
    visible = value; // maybe use parent setVisible(...)? would be better but which one?
    if (value == true)
    {
        resizeItems();   // move items in box in proper places
        setNavigation(); // set navigation through kids -> TODO handle out of last/first to parent
        if (children.size())
        { // set first visible kid as focused item - TODO should check for actionItems too...
            /// this if back / front is crappy :|
            if (previous)
            {
                for (auto el = children.rbegin(); el != children.rend(); ++el)
                {
                    if ((*el)->visible && (*el)->activeItem)
                    {
                        setFocusItem(*el);
                        break;
                    }
                }
            }
            else
            {
                for (auto &el : children)
                {
                    if (el->visible && el->activeItem)
                    {
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

template <BoxLayout::BoxAxis axis> void BoxLayout::resizeItems()
{
    if (children.empty())
    {
        return;
    }
    uint32_t freeSpace = inAxisSpace(axis, this);
    int16_t offsetInAxis = 0;
    uint16_t movedInAxis = 0;

    // accessor lambdas, depending on if it's in Y or X axiss we want to access in x or Y
    auto setPosInAxis = [this](Item *it, uint16_t off) {
        if (this->reverse_order)
        {
            off = inAxisSpace(axis, this) - this->inAxisMax(axis, it) - off;
        }
        // LOG_DEBUG("Box [%d] move in axis %s to %u", this->inAxisMax(axis,it), axis==BoxLayout::BoxAxis::Y?"Y":"X", off);
        if (axis == BoxAxis::Y)
        {
            it->widgetArea.y = off;
        }
        else
        {
            it->widgetArea.x = off;
        }
    };

    auto setSizeInAxis = [](Item *it, uint16_t val) {
        // LOG_DEBUG("Size in axis %s to %u", axis==BoxLayout::BoxAxis::Y?"Y":"X", val);
        auto oldbox = it->widgetArea;
        if (axis == BoxAxis::Y)
        {
            it->widgetArea.h = val;
        }
        else
        {
            it->widgetArea.w = val;
        }
        auto newbox = it->widgetArea;
        it->onDimensionChanged(oldbox, newbox);
    };

    // create list of elements to be modified. Each element is placed in the BoxElement structure;
    // width of all elements are set to 0
    std::list<BoxElement> boxElements;
    for (Item *item : children)
    {
        // ignore not visible elements
        if (item->visible)
        {
            boxElements.push_back(BoxElement(item));
            inAxisSize(axis, item) = 0;
        }
    }

    uint32_t elementsToChange = boxElements.size();
    while (freeSpace > 0)
    {
        offsetInAxis = 0;
        uint16_t expandValue = freeSpace / elementsToChange;

        // for case when freeSpace is greater than 0 but smaller than elementsToChange
        // expand value is set to 1
        if (expandValue == 0)
            expandValue = 1;

        std::list<BoxElement>::iterator i = boxElements.begin();

        while ((i != boxElements.end()) && (freeSpace > 0))
        {
            // element was already updated in previous run and no further modification is possible or required
            if (i->noUpdate)
            {
                setPosInAxis(i->item, offsetInAxis);
                offsetInAxis += inAxisSize(axis, i->item);
                ++i;
                continue;
            }
            else if (inAxisSize(axis, i->item) == inAxisMax(axis, i->item))
            {
                movedInAxis = inAxisSize(axis, i->item);
                offsetInAxis += movedInAxis;
                freeSpace -= movedInAxis;
                setPosInAxis(i->item, offsetInAxis);
                setSizeInAxis(i->item, movedInAxis);
                i->noUpdate = true;
                elementsToChange--;
            }
            else
            {
                uint16_t maxInAxis = inAxisMax(axis, i->item);
                movedInAxis = inAxisSize(axis, i->item) + expandValue;

                if (movedInAxis > maxInAxis)
                {
                    movedInAxis = maxInAxis;
                    elementsToChange--;
                    i->noUpdate = true;
                }

                freeSpace -= (movedInAxis - inAxisSize(axis, i->item));
                setPosInAxis(i->item, offsetInAxis);
                setSizeInAxis(i->item, movedInAxis);

                offsetInAxis += movedInAxis;
            }

            ++i;
        }

        if (elementsToChange <= 0)
        {
            break;
        }
    }

    Rect::updateDrawArea();
}

/// this actually should be addWidget - but there are noChecks nowhere when addWidget is being used...
template <BoxLayout::BoxAxis axis> bool BoxLayout::tryAddWidget(Item *item)
{

    if (children.size() == 0)
    {
        // get max in Axis
        if (inAxisSpace(axis, this) > inAxisMax(axis, item))
        {
            return addWidget(item);
        }
        else
        {
            LOG_ERROR("Element too big to even try to fit in! %d > %d", inAxisSpace(axis, this), inAxisMax(axis, item));
            return false;
        }
    }
    else
    {
        if (inAxisSpace(axis, this) - inAxisOffset(axis, children.back()) > inAxisMax(axis, item))
        {
            LOG_DEBUG("add  ->: %d - %d > %d", inAxisSpace(axis, this), inAxisOffset(axis, children.back()), inAxisMax(axis, item));
            return addWidget(item);
        }
        else
        {
            /// there is no space for next element
            LOG_ERROR("Not enough space!  ->: ! %d - %d > %d [%d]", inAxisSpace(axis, this), inAxisOffset(axis, children.back()), inAxisMax(axis, item),
                      children.size());
            return false;
        }
    }
    return false;
}

std::list<Item *>::iterator BoxLayout::nextNavigationItem(std::list<Item *>::iterator from)
{
    return std::find_if(from, this->children.end(), [](auto &el) -> bool {
        if (el->visible && el->activeItem)
        {
            return true;
        }
        return false;
    });
}

void BoxLayout::setNavigation()
{
    auto previous = nextNavigationItem(children.begin()), next = children.end();
    int i = 0;
    while ((previous != children.end()) && ((next = nextNavigationItem(std::next(previous))) != std::end(children)))
    {
        (*previous)->setNavigationItem(reverse_order ? NavigationDirection::UP : NavigationDirection::DOWN, *next);
        (*next)->setNavigationItem(reverse_order ? NavigationDirection::DOWN : NavigationDirection::UP, *previous);
        previous = next;
    }
}

HBox::HBox() : BoxLayout() {
	type = ItemType::HBOX;
}
HBox::HBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h) :
	BoxLayout( parent, x, y, w, h ) {
	type = ItemType::HBOX;
}

void HBox::resizeItems() {
    BoxLayout::resizeItems<BoxLayout::BoxAxis::X>();
}

bool HBox::tryAddWidget(Item *item)
{
    return BoxLayout::tryAddWidget<BoxLayout::BoxAxis::X>(item);
}

VBox::VBox() : BoxLayout() {
	type = ItemType::VBOX;
}
VBox::VBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h) :
	BoxLayout( parent, x, y, w, h ) {
	type = ItemType::VBOX;
}

void VBox::resizeItems() {
    BoxLayout::resizeItems<BoxLayout::BoxAxis::Y>();
}

bool VBox::tryAddWidget(Item *item)
{
    return BoxLayout::tryAddWidget<BoxLayout::BoxAxis::Y>(item);
}

} /* namespace gui */
