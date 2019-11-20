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

	text = new gui::Label( this, 0,0,0,0);
	text->setPenFocusWidth(0);
	text->setPenWidth(0);
	text->setFont(style::window::font::big);
	text->setDotsMode( true );
	text->setAlignement( gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER} );
	text->setText("item");
}

bool ThreadItem::onDimensionChanged(const BoundingBox& oldDim, const BoundingBox& newDim)
{

	return true;
}
} /*namespace gui*/



