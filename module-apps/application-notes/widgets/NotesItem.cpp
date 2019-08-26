/*
 * @file NotesItem.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 12 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "NotesItem.hpp"

namespace gui {

NotesItem::NotesItem(NotesModel* model, bool mode24H) : model{model}, mode24H{mode24H} {
	minWidth = 436;
	minHeight = 146;
	maxWidth = 436;
	maxHeight = 146;

	setRadius( 8 );

	setPenFocusWidth(3);
	setPenWidth(1);

	hour = new gui::Label( this, 0,0,0,0);
	hour->setPenFocusWidth(0);
	hour->setPenWidth(0);
	hour->setFont("gt_pressura_regular_24");
	hour->setAlignement(gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_TOP } );

	title = new gui::Label( this, 0,0,0,0);
	title->setPenFocusWidth(0);
	title->setPenWidth(0);
	title->setFont("gt_pressura_bold_24");
	title->setAlignement(gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_TOP } );

	snippet = new gui::Label( this, 0,0,0,0);
	snippet->setPenFocusWidth(0);
	snippet->setPenWidth(0);
	snippet->setFont("gt_pressura_regular_16");
	snippet->setAlignement(gui::Alignment { gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER} );

}

NotesItem::~NotesItem() {
}

bool NotesItem::onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) {
	hour->setPosition(11, 0 );
	hour->setSize(newDim.w-22, 40 );

	title->setPosition(11, 0 );
	title->setSize(68, 40 );

	snippet->setPosition(11, 40 );
	snippet->setSize(newDim.w-22, newDim.h-40 );
	return true;
}

//sets copy of alarm's
void NotesItem::setNote( std::shared_ptr<NotesRecord> note ) {
	this->note = note;
	//set values of the labels
	title->setText( std::to_string( note->ID));
	snippet->setText( note->path );
}

bool NotesItem::onActivated( void* data ) {
	LOG_INFO("ITEM WAS PRESSED");
	return true;
}


} /* namespace gui */
