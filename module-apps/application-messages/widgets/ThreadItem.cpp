/*
 * ThreadItem.cpp
 *
 *  Created on: 15 lis 2019
 *      Author: kuba
 */
#include "ThreadItem.hpp"

#include <Style.hpp>

namespace gui{

ThreadItem::ThreadItem(ThreadModel* model)
{

	setRadius( 0 );
	setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP );

	setPenFocusWidth(2);
	setPenWidth(0);

}

bool ThreadItem::onDimensionChanged(const BoundingBox& oldDim, const BoundingBox& newDim)
{

	return true;
}
} /*namespace gui*/



