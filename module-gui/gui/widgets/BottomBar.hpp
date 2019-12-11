/*
 * BottomBar.hpp
 *
 *  Created on: 13 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_BOTTOMBAR_HPP_
#define MIDDLEWARES_GUI_WIDGETS_BOTTOMBAR_HPP_

#include "Rect.hpp"
#include "Label.hpp"
#include "utf8/UTF8.hpp"

namespace gui {

class BottomBar: public Rect {
public:
	enum class Side {
		LEFT = 0x01,
		CENTER,
		RIGHT
	};
protected:
	Label* left;
	Label* center;
	Label* right;
	gui::Label* prepareLabel( BottomBar::Side side );
    Label *getSide(BottomBar::Side side);

  public:

	BottomBar();
	BottomBar( Item* parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h );
	virtual ~BottomBar();

	void setActive( BottomBar::Side side, bool active );
	void setText( BottomBar::Side side, const UTF8& str, bool active = true );
    UTF8 getText(BottomBar::Side side);

    //virtual methods from Item
	bool onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim);
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_BOTTOMBAR_HPP_ */
