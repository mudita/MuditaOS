/*
 * Rect.h
 *
 *  Created on: 6 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_RECT_HPP_
#define MIDDLEWARES_GUI_WIDGETS_RECT_HPP_

#include <list>
#include <cstdint>
#include "Item.hpp"
#include "../core/Color.hpp"

namespace gui {

class Rect: public Item {
public:
	//defines color used for drawing border
	Color borderColor;
	//defines color for filling rectangle
	Color fillColor;
	//defines number of pixels used to draw border of rectangle without focus
	uint8_t penWidth;
	//defines number of pixels used to draw border of rectangle when it has focus
	uint8_t penFocusWidth;
	//defines whether rectangle should be filled with fill color.
	bool filled;
	//flags that defines whether paint given border
	RectangleEdgeFlags edges;
	//flags that defines which edge should be flat. This will disable roundness on both sides of the edge.
	RectangleFlatFlags flatEdges;
	//flags that defines whether paint given corner (only for rounded corners)
	RectangleCornerFlags corners;
	//flags that define whether to paint japs. small protrusions indicating a speech bubble
	RectangleYapFlags yaps;
    const unsigned short YAPS_SIZE_DEFAULT = 10;
    // a yap size (if present). size is horizontal width.
	unsigned short yapSize = YAPS_SIZE_DEFAULT;

  public:

	Rect();
	Rect( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h);
	virtual ~Rect();

	void setFillColor( const Color& color );
	void setBorderColor( const Color& color );
	void setPenWidth( uint8_t width );
	void setPenFocusWidth( uint8_t width );
	void setEdges( RectangleEdgeFlags edges );
	void setCorners( RectangleCornerFlags corners );
	void setFlat( RectangleFlatFlags flats);
    void setYaps( RectangleYapFlags yaps);
    void setYapSize( short value);
    void setFilled( bool val );

	//virtaul methods
	std::list<DrawCommand*> buildDrawList();
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_RECT_HPP_ */
