/*
 * @file CalllogItem.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log Item
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "CalllogItem.hpp"
#include "time/time_conversion.hpp"
#include "../data/CallLogInternals.hpp"

#include <Style.hpp>

using namespace calllog;

namespace gui {

CalllogItem::CalllogItem(CalllogModel* model, bool mode24H) : model{model}, mode24H{mode24H} {
	
	minWidth = 431;
	minHeight = 55;
	maxWidth = 431;
	maxHeight = 55;

	setRadius( 0 );
	setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP );

	setPenFocusWidth(2);
	setPenWidth(0);

	timestamp = new gui::Label(this, 0,0,0,0);
	timestamp->setPenFocusWidth(0);
	timestamp->setPenWidth(0);
	timestamp->setFont(style::window::font::small);
	timestamp->setAlignement(gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_CENTER } );

	auto newImg = [=](const UTF8 imageName)->gui::Image* { 
		auto img = new gui::Image(this, 11, 22, 0, 0, imageName); 
		img->setVisible(false); 
		return img;  
	};
	imageCallType[calllog::CallLogCallType::IN] = newImg("calllog_arrow_in");
	imageCallType[calllog::CallLogCallType::OUT] = newImg("calllog_arrow_out");
	imageCallType[calllog::CallLogCallType::MISSED] = newImg("calllog_arrow_den");

	text = new gui::Label( this, 0,0,0,0);
	text->setPenFocusWidth(0);
	text->setPenWidth(0);
	text->setFont(style::window::font::big);
	text->setDotsMode( true );
	text->setAlignement( gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER} );
}

CalllogItem::~CalllogItem() {
	call = nullptr;
	if( timestamp ) { removeWidget(timestamp); delete timestamp; timestamp = nullptr;}
	for( auto& img : imageCallType ) {
		if( img ) { removeWidget(img); delete img; img = nullptr; }
	}	
	if( text ) { removeWidget(text); delete text; text = nullptr;}
}

bool CalllogItem::onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) {
	text->setPosition(33, 0 );
	text->setSize(newDim.w-33-100, newDim.h);

	timestamp->setPosition(newDim.w-100,0);
	timestamp->setSize(100, newDim.h);
	return true;
}

void CalllogItem::setCall( std::shared_ptr<CalllogRecord>& call ) {
	this->call = call;
	text->setText( call->name );

	auto callType = calllog::toCallLogCallType(call->type);
	if (callType == calllog::CallLogCallType::MISSED) text->setFont(style::window::font::bigbold);

	imageCallType[static_cast<uint32_t>(callType)]->setVisible(true);

	timestamp->setText( utils::time::SysTime(call->date, false)); // TODO: alek: check for AM/PM and 24h
}

} /* namespace gui */
