/*
 * @file PhonebookListView.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 11 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "log/log.hpp"
#include "PhonebookListView.hpp"

namespace gui {

PhonebookListView::PhonebookListView() : ListView(){
	orientation = ORIENTATION_TOP_DOWN;
}

PhonebookListView::PhonebookListView( Item* parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h ) :
	ListView( parent, x, y, w, h ) {
	orientation = ORIENTATION_TOP_DOWN;
}

PhonebookListView::~PhonebookListView() {
}

bool PhonebookListView::onInput( const InputEvent& inputEvent ) {
	if( (inputEvent.state == InputEvent::State::keyReleasedShort ) && (inputEvent.keyCode == KeyCode::KEY_ENTER) ) {
		return onActivated(nullptr);
	}

	if( (inputEvent.state == InputEvent::State::keyReleasedShort ) && (inputEvent.keyCode == KeyCode::KEY_UP) ){
		if( orientation == ORIENTATION_TOP_DOWN ) {
			if( listMode == MODE_PAGE ){
				if( selectedIndex > firstIndex ) {
					selectedIndex--;
					return true;
				}
				if( selectedIndex == firstIndex ) {
					orientation = ORIENTATION_BOTTOM_UP;
					if( firstIndex > 0 ) {
						firstIndex -= pageSize;
						if( firstIndex < 0 ) {
							orientation = ORIENTATION_TOP_DOWN;
							firstIndex = 0;
						}
						selectedIndex = lastIndex;
//						lastIndex = firstIndex + pageSize -1;
//						if( lastIndex > elementsCount - 1 )
//							lastIndex = elementsCount - 1;
//
						updateScrollDimenstions();
						return true;
					}
				}
			}
			else {
				//TODO implement continuous mode
			}
		}
		else if( orientation == ORIENTATION_BOTTOM_UP ) {
			//TODO implement BOTTOM_UP orientation
		}
	}
	else if( (inputEvent.state == InputEvent::State::keyReleasedShort ) && (inputEvent.keyCode == KeyCode::KEY_DOWN) ){
		if( orientation == ORIENTATION_TOP_DOWN ) {
			if( listMode == MODE_PAGE ) {
				if( selectedIndex < lastIndex ) {
					selectedIndex++;
					return true;
				}
				if( selectedIndex == lastIndex ) {
					orientation = ORIENTATION_TOP_DOWN;
					if( lastIndex < elementsCount - 1 ) {
						firstIndex = lastIndex + 1;
						if( firstIndex > elementsCount - 1 ) {
							firstIndex = elementsCount - 1;
						}
						selectedIndex = firstIndex;
						//lastIndex = firstIndex + pageSize -1;
//						if( lastIndex > elementsCount - 1 )
//							lastIndex = elementsCount - 1;
//
						updateScrollDimenstions();
						return true;
					}
				}
			}
			else { //continuous mode

			}
		}
		else { // BOTTOM_UP

		}
	}
	return false;
}

void PhonebookListView::updatePageItems() {

	// for top down orientation
	if( orientation == ORIENTATION_TOP_DOWN ) {
		//index of requested item is incremented only if payload of returned item is not null
		//if it's null it means that this is favourite label of label connected with showing Capital latters for surnames
		int index = firstIndex, lastIndex = -1;
		this->lastIndex = firstIndex;
		for( int i=0; i<pageSize; i++ ) {
			ListItem* item = provider->getItem(index, firstIndex, lastIndex, pageSize );

			if( item != nullptr ) {
				addWidget(item);
				items.push_back(item);

				lastIndex = index;
				if( item->getID() >= 0 ) {
					index++;
					this->lastIndex++;
				}
			}
		}
		//after the loop lastIndex is the number of elements not the index of last element that's why -- is used.
		this->lastIndex--;
	}
	else if( orientation == ORIENTATION_BOTTOM_UP ) {

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

	auto it = items.begin();
	for(unsigned int i=0; i<items.size(); i++ ) {
		if( availableHeight > 0 ) {

			itemHeight = (*it)->minHeight;

			if( orientation == ORIENTATION_TOP_DOWN ) {
				(*it)->setPosition(0, verticalPosition );
				verticalPosition += itemHeight + 1; //1 for separator
			}
			else {
				verticalPosition -= itemHeight - 1; //1 for separator
				(*it)->setPosition(0, verticalPosition );
			}

			(*it)->setSize(itemWidth, itemHeight );

			//if list has focus and it is visible mark selected element
			if( visible ) {
				if( (*it)->getID() == selectedIndex )
					(*it)->setFocus(true);
			}

			availableHeight -=itemHeight;
		}
		std::advance( it, 1 );
	}
}

} /* namespace gui */
