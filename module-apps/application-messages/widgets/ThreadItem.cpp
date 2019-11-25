/*
 * ThreadItem.cpp
 *
 *  Created on: 15 lis 2019
 *      Author: kuba
 */
#include "ThreadItem.hpp"
#include "time/time_conversion.hpp"
#include <Style.hpp>
#include "service-db/api/DBServiceAPI.hpp"

namespace gui{

ThreadItem::ThreadItem(ThreadModel* threadmodel)
{
	model = threadmodel;
	minWidth = 431;
	minHeight = 100;
	maxWidth = 431;
	maxHeight = 100;

	setRadius( 0 );
	setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP );

	setPenFocusWidth(2);
	setPenWidth(0);

	contact = new gui::Label( this, 0,0,0,0);
	contact->setPenFocusWidth(0);
	contact->setPenWidth(0);
	contact->setFont(style::window::font::big);
	contact->setDotsMode( true );
	contact->setAlignement( gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER} );

	timestamp = new gui::Label( this, 0,0,0,0);
	timestamp->setPenFocusWidth(0);
	timestamp->setPenWidth(0);
	timestamp->setFont(style::window::font::small);
	timestamp->setDotsMode( true );
	timestamp->setAlignement( gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_CENTER} );

	preview = new gui::Label( this, 0,0,0,0);
	preview->setPenFocusWidth(0);
	preview->setPenWidth(0);
	preview->setFont(style::window::font::small);
	preview->setDotsMode( true );
	preview->setAlignement( gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER} );

}

bool ThreadItem::onDimensionChanged(const BoundingBox& oldDim, const BoundingBox& newDim)
{
	contact->setPosition(14, 10 );
	contact->setSize(newDim.w-33-100, newDim.h/2);

	timestamp->setPosition(newDim.w-100,0);
	timestamp->setSize(100, newDim.h/2);

	preview->setPosition(14, newDim.h/2);
	preview->setSize(newDim.w-20, newDim.h/2);


	return true;
}

void ThreadItem::setThreadItem(std::shared_ptr<ThreadRecord>& thread)
{
	this->thread = thread;

	if( model != nullptr )
	{
		auto contactRec = DBServiceAPI::ContactGetByID(model->getApplication(), thread->contactID);
		auto cont = contactRec->front();
		contact->setText(cont.primaryName + " " + cont.alternativeName);
	}

	timestamp->setText( utils::time::SysTime(thread->date) );

	preview->setText(thread->snippet);

}

} /*namespace gui*/



