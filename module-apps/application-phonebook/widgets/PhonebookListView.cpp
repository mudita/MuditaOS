/*
 * @file PhonebookListView.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 11 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "PhonebookListView.hpp"

namespace gui {

PhonebookListView::PhonebookListView() : ListView(){
}

PhonebookListView::PhonebookListView( Item* parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h ) :
	ListView( parent, x, y, w, h ) {
}

PhonebookListView::~PhonebookListView() {
}

bool PhonebookListView::onInput( const InputEvent& inputEvent ) {
	return ListView::onInput(inputEvent);
}

void PhonebookListView::updatePageItems() {

	//index of requested item is incremented only if payload of returned item is not null
	//if it's null it means that this is favourite label of label connected with showing Capital latters for surnames
	int index = firstIndex, lastIndex = -1;
	for( int i=0; i<pageSize; i++ ) {
		ListItem* item = provider->getItem(index, firstIndex, lastIndex, pageSize );

		if( item != nullptr ) {
			addWidget(item);
			items.push_back(item);
			if( item->getID() >= 0 ) {
				index++;
			}
			lastIndex = index;
		}
	}

	//calculate height of the item using list's height and pageSize
	uint32_t itemWidth = widgetArea.w;
	if( drawVerticalScroll )
		itemWidth -= 10;

	int availableHeight  = widgetArea.h - pageSize;
	if( availableHeight < 0 )
		availableHeight = 0;
	int itemHeight = availableHeight / pageSize;

	int verticalPosition;
	if( orientation == ORIENTATION_TOP_DOWN )
		verticalPosition = 0;
	else
		verticalPosition = widgetArea.h;

	for(unsigned int i=0; i<items.size(); i++ ) {
		if( availableHeight > 0 ) {

			itemHeight = items[i]->minHeight;

			if( orientation == ORIENTATION_TOP_DOWN ) {
				items[i]->setPosition(0, verticalPosition );
				verticalPosition += itemHeight + 1; //1 for separator
			}
			else {
				verticalPosition -= itemHeight - 1; //1 for separator
				items[i]->setPosition(0, verticalPosition );
			}

			items[i]->setSize(itemWidth, itemHeight );

			//if list has focus and it is visible mark selected element
			if( visible ) {
				if( (int)(i+firstIndex) == selectedIndex )
					items[i]->setFocus(true);
			}

			availableHeight -=itemHeight;
		}
	}
}

} /* namespace gui */
