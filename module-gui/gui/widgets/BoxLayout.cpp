/*
 * BoxLayout.cpp
 *
 *  Created on: 16 maj 2019
 *      Author: robert
 */

#include "BoxLayout.hpp"

namespace gui {

BoxLayout::BoxLayout() : Rect() {

}
BoxLayout::BoxLayout( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h) :
	Rect ( parent, x, y, w, h ){
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

template <BoxLayout::BoxAxis axis> void BoxLayout::resizeItems()
{
    if (children.empty())
    {
        // LOG_ERROR("Resize empty box!");
        return;
    }

    uint32_t freeSpace = axis == BoxAxis::Y ? widgetArea.h : widgetArea.w;
    int16_t offsetInAxis = 0;
    uint16_t maxInAxis;
    uint16_t movedInAxis;

    // accessor lambdas, depending on if it's in Y or X axiss we want to access in x or Y
    auto setPosInAxis = [=](Item *it) {
        if (axis == BoxAxis::Y)
        {
            it->widgetArea.y = offsetInAxis;
        }
        else
        {
            it->widgetArea.x = offsetInAxis;
        }
    };

    auto setSizeInAxis = [=](Item *it) {
        auto oldbox = it->widgetArea;
        if (axis == BoxAxis::Y)
        {
            it->widgetArea.h = movedInAxis;
        }
        else
        {
            it->widgetArea.w = movedInAxis;
        }
        auto newbox = it->widgetArea;
        it->onDimensionChanged(oldbox, newbox);
    };

    auto inAxisPos = [=](Item *it) -> uint16_t & { return (axis == BoxAxis::Y) ? (it->widgetArea.h) : (it->widgetArea.w); };

    auto getMaxInAxis = [=](Item *it) { return (axis == BoxAxis::Y) ? (it->getMaxHeight()) : (it->getMaxWidth()); };

    // create list of elements to be modified. Each element is placed in the BoxElement structure;
    // width of all elements are set to 0
    std::list<BoxElement> boxElements;
    for (Item *item : children)
    {
        boxElements.push_back(BoxElement(item));
        inAxisPos(item) = 0;
    }

    uint32_t elementsToChange = children.size();
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
                setPosInAxis(i->item);
                offsetInAxis += inAxisPos(i->item);
                ++i;
                continue;
            }
            else if (inAxisPos(i->item) == getMaxInAxis(i->item))
            {
                movedInAxis = inAxisPos(i->item);
                offsetInAxis += movedInAxis;
                freeSpace -= movedInAxis;
                setPosInAxis(i->item);
                setSizeInAxis(i->item);
                i->noUpdate = true;
                elementsToChange--;
            }
            else
            {
                maxInAxis = getMaxInAxis(i->item);
                movedInAxis = inAxisPos(i->item) + expandValue;

                if (movedInAxis > maxInAxis)
                {
                    movedInAxis = maxInAxis;
                    elementsToChange--;
                    i->noUpdate = true;
                }

                freeSpace -= (movedInAxis - inAxisPos(i->item));
                setPosInAxis(i->item);
                setSizeInAxis(i->item);

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

} /* namespace gui */
