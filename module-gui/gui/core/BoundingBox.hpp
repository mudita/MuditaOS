/*
 * BoundingBox.h
 *
 *  Created on: 4 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_BOUNDINGBOX_HPP_
#define MIDDLEWARES_GUI_WIDGETS_BOUNDINGBOX_HPP_

#include <cstdint>

namespace gui {

class BoundingBox {
public:
	int16_t x,y;
	uint16_t w,h;
	BoundingBox();
	BoundingBox( int16_t x, int16_t y, uint16_t w, uint16_t h);
	virtual ~BoundingBox();

	static bool intersect( const BoundingBox& box1, const BoundingBox& box2, BoundingBox& result );

	//inline
	void clear() { x=0; y=0; w=0; h=0; };

};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_BOUNDINGBOX_HPP_ */
