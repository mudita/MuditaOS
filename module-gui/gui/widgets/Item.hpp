/*
 * Item.hpp
 *
 *  Created on: 4 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_ITEM_HPP_
#define MIDDLEWARES_GUI_WIDGETS_ITEM_HPP_

#include <vector>
#include <list>
#include <functional>

#include "KeyEvent.hpp"
#include "Common.hpp"
#include "core/DrawCommand.hpp"
#include "core/BoundingBox.hpp"
#include "input/InputEvent.hpp"
#include "Layout.hpp"
#include "Navigation.hpp"
#include "Margins.hpp"

namespace gui {

class Navigation;

enum class ItemType {
	ITEM = 0,
	RECT = 1,
	LABEL,
	LIST,
	LIST_ITEM,
	TEXT,
	IMAGE,
	LAYOUT,
	VBOX,
	HBOX
};

class Item {
public:
	//flag that informs whether item has a focus
	bool focus;
    // pointer to the child item that has focus
    Item *focusItem = nullptr;
    // type of the widget
    ItemType type;
	//pointer to the parent Item
    Item *parent = nullptr;
    // list of items that have the same parent.
    std::list<Item*> children;
    enum class Area
    {
        Normal,
        Draw,
        Actual, // actual area in use between: Normal <-> Max
        Max,
    };
    /// bounding box of the item. This is in coordinates of the parent widget.
    BoundingBox widgetArea;
    /// bounding box of the item maximal size, if not specified other - the same as widget Area
    BoundingBox widgetMaxArea;
    /// bounding box of the item maximal size, if not specified other - the same as widget Area
    BoundingBox widgetActualArea;
    //bounding box used for drawing. This is in coordinates of window
	BoundingBox drawArea; // drawableArea would be more accurate
                          // maximal bounding box size
    auto area(Area which = Area::Normal) -> BoundingBox &
    {
        switch (which)
        {
        case Area::Normal:
            return widgetArea;
        case Area::Draw:
            return drawArea;
        case Area::Max:
            return widgetMaxArea;
        case Area::Actual:
            return widgetActualArea;
        }
        return widgetArea;
    }
    Margins innerMargins;
    //radius of corner
	short radius;
    // flag that defines if item is active
    // if false -> than it shouldn't be used witn onInput, navitagion etc.
    bool activeItem = true;
    //flag that defines whether widget is visible
	bool visible;
	//policy for changing vertical size if Item is placed inside layout
	LayoutVerticalPolicy verticalPolicy;
	//policy for changing horizontal size if Item is placed inside layout
	LayoutHorizontalPolicy horizontalPolicy;
	//Maximum height to which Layout base widget can scale current widget
	uint16_t maxHeight;
	//Maximum width to which Layout base widget can scale current widget
	uint16_t maxWidth;

	//callbacks
	std::function<bool(Item&)> focusChangedCallback;
	std::function<void(Item&,  void* data)> dimensionChangedCallback;
	std::function<bool(Item&)> activatedCallback;
	std::function<bool(Item&, const InputEvent& inputEvent)> inputCallback;
	std::function<bool(Item&)> contentCallback;
    std::function<bool(const InputEvent &)> itemNavigation = nullptr;

    bool handleNavigation(const InputEvent inputEvent);

    int16_t w() { return widgetArea.w; }
    int16_t h() { return widgetArea.h; }
    /// helper function to show where widget ends in x axis
    int32_t offset_w() { return w() + widgetArea.x; }
    /// helper function to show where widget ends in y axis
    int32_t offset_h() { return h() + widgetArea.y; }
    int32_t offset(Axis axis)
    {
        return this->widgetArea.size(axis) + this->widgetArea.pos(axis);
    };

    bool setFocus(bool state)
    {
        if (state != focus)
        {
            focus = state;
            onFocus( state );
			focusChangedCallback( *this );
        };
        return state;
    }

    void setFocusItem(Item *item)
    {
        auto checknrun = [=](bool on) {
            if (focusItem != nullptr)
            {
                focusItem->setFocus(on);
            }
        };
        checknrun(false);
        focusItem = item;
        checknrun(true);
    }

    Item *getFocusItem()
    {
        return focusItem;
    }

    virtual bool onFocus(bool state)
    {
        focus = state;
        return true;
    };
    virtual bool onActivated(void *data)
    {
        if (activatedCallback)
            return activatedCallback(*this);
        else
            return false;
    };
    virtual bool onInput( const InputEvent& inputEvent ) { return inputCallback( *this, inputEvent ); };
	virtual bool onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) { return true; };
	virtual bool onContent() { return false; };

	virtual bool addWidget( Item* item );
	virtual bool removeWidget( Item* item );
	virtual void setVisible( bool value );
	virtual void setPosition( const short& x, const short& y );
    virtual void setSize(const unsigned short w, const unsigned short h);
    virtual void setBoundingBox(const BoundingBox &new_box);
	virtual std::list<DrawCommand*> buildDrawList();
	virtual void setRadius( int value );

	//functinos to handle navigation
	virtual Item* getNavigationItem( NavigationDirection direction );
	virtual void setNavigationItem( NavigationDirection direction, Item* item );

	Item();
	virtual ~Item();

	void setX(const uint32_t x);
	void setY(const uint32_t y);
    [[nodiscard]] const uint32_t getX() const
    {
        return (widgetArea.x);
    }
    [[nodiscard]] const uint32_t getY() const
    {
        return (widgetArea.y);
    }
    [[nodiscard]] const uint32_t getWidth() const
    {
        return (widgetArea.w);
    }
    [[nodiscard]] const uint32_t getHeight() const
    {
        return (widgetArea.h);
    }

protected:
	//On change of position or size this method will recalculate visible part of the widget
	//considering widgets hierarchy and calculate absolute position of drawing primitives.
	virtual void updateDrawArea();
    /**
     * Pointer to navigation object. It is added when object is set for one of the directions
     */
    gui::Navigation* navigationDirections;
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_ITEM_HPP_ */
