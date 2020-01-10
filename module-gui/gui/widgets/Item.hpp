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
	//bounding box of the item. This is in coordinates of the parent widget.
	BoundingBox widgetArea;
	//bounding box used for drawing. This is in coordinates of window
	BoundingBox drawArea;
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
	//Minimal height to which Layout base widget can scale current widget
	uint16_t minHeight;
	//Minimal width to which Layout base widget can scale current widget
	uint16_t minWidth;
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

	bool setFocus( bool state ) {
		if( state != focus ) {
			//focus = state;
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
	virtual void setSize( const short& w, const short& h );
    virtual void setBoundingBox(const BoundingBox &new_box);
	virtual std::list<DrawCommand*> buildDrawList();
	virtual void setRadius( int value );

	//functinos to handle navigation
	virtual Item* getNavigationItem( NavigationDirection direction );
	virtual void setNavigationItem( NavigationDirection direction, Item* item );

	Item();
	virtual ~Item();

	/**
	 * @brief Method returns minimal height for the widget.
	 * @return Unsigned value between 0 and 0xFFFF.
	 * @note This method is used by containers that inherit from the Layout class (HBox, VBox...).
	 * This value takes into consideration specific elements of widget like font's height, rounded corners and border's width.
	 */
	uint32_t getMinHeight();
	/**
	 * @brief Method returns minimal width for the widget.
	 * @return Unsigned value between 0 and 0xFFFF.
	 * @note This method is used by containers that inherit from the Layout class (HBox, VBox...).
	 * This value takes into consideration specific elements of widget like rounded corners and border's width.
	 */
	uint32_t getMinWidth();
	/**
	 * @brief Method returns maximum height for the widget.
	 * @return Unsigned value between 0 and 0xFFFF.
	 * @note Returned value must be equal or greater than value returned by getMinHeight.
	 */
	uint32_t getMaxHeight();
	/**
	 * @brief Method returns maximum width for the widget.
	 * @return Unsigned value between 0 and 0xFFFF.
	 * @note Returned value must be equal or greater than value returned by getMinWidth.
	 */
	uint32_t getMaxWidth();

	void setMaxSize( const uint16_t& w, const uint16_t& h);
	void setMinSize( const uint16_t& w, const uint16_t& h);
	void setX(const uint32_t x);
	void setY(const uint32_t y);
	const uint32_t getX() const			{ return (widgetArea.x); }
	const uint32_t getY() const			{ return (widgetArea.y); }
	const uint32_t getWidth() const		{ return (widgetArea.w); }
	const uint32_t getHeight() const	{ return (widgetArea.h); }
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
