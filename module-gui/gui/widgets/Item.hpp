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
	LAYOUT
};

class Item {
public:
	//flag that informs whether item has a focus
	bool focus;
	//type of the widget
	ItemType type;
	//pointer to the parent Item
	Item* parent;
	//list of items that have the same parent.
	std::list<Item*> children;
	//bounding box of the item. This is in coordinates of the parent widget.
	BoundingBox widgetArea;
	//bounding box used for drawing. This is in coordinates of window
	BoundingBox drawArea;
	//radius of corner
	short radius;
	//flag that defines whether widget is enabled
	bool enabled;
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
	std::function<bool(Item&, InputEvent& inputEvent)> inputCallback;

	bool setFocus( bool state ) {
		if( state != focus ) {
			//focus = state;
			onFocus( state );
			focusChangedCallback( *this );
		};
		return state;
	}

	virtual bool onFocus( bool state ) { focus = state; return true; };
	virtual bool onActivated( void* data ) { return activatedCallback(*this); };
	virtual bool onInput( const InputEvent& inputEvent ) { return false; };
	virtual bool onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) { return true; };

	virtual bool addWidget( Item* item );
	virtual bool removeWidget( Item* item );
	virtual void setVisible( bool value );
	virtual void setPosition( const short& x, const short& y );
	virtual void setSize( const short& w, const short& h );
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
