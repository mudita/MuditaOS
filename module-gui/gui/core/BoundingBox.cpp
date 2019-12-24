/*
 * BoundingBox.cpp
 *
 *  Created on: 4 mar 2019
 *      Author: robert
 */

#include "BoundingBox.hpp"

namespace gui {

BoundingBox::BoundingBox() : x {0}, y{0}, w{0}, h{0} {
}

BoundingBox::BoundingBox(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

BoundingBox::~BoundingBox() {
}

bool BoundingBox::intersect( const BoundingBox& box1, const BoundingBox& box2, BoundingBox& result ) {
	const BoundingBox* l;
	const BoundingBox* r;

	if( box1.x < box2.x ) {
		l = &box1; r = &box2;
	}
	else {
		l = &box2; r = &box1;
	}

	int w = l->x + l->w - r->x;
	if( w <= 0 )
	{
		result.clear();
		return false;
	}
	//most left bbox overlap right box entirely
	if( (l->x + l->w) >= (r->x + r->w) )
	{
		w = r->w;
	}
	result.w = w;
	result.x = r->x;

	//vertical check
	//select bbox that is higher
	const BoundingBox* u;
	if( box1.y < box2.y ) {
		l = &box1; u = &box2;
	}
	else {
		l = &box2; u = &box1;
	}

	int h = l->y + l->h - u->y;

	if( h <= 0 )
	{
		result.clear();
		return false;
	}

	//most lower bbox overlap upper box entirely
	if( (l->y + l->h) >= (u->y + u->h) )
	{
		h = u->h;
	}
	result.y = u->y;
	result.h = h;

	return true;
}

} /* namespace gui */
