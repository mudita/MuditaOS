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
//					orientation = ORIENTATION_BOTTOM_UP;
					if( firstIndex > 0 ) {
						selectedIndex = firstIndex - 1;
						lastIndex = selectedIndex;
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
			if( listMode == MODE_PAGE ){
				if( selectedIndex > firstIndex ) {
					selectedIndex--;
					return true;
				}
				if( selectedIndex == firstIndex ) {
//					orientation = ORIENTATION_TOP_DOWN;
					if( firstIndex > 0 ) {
						selectedIndex = firstIndex - 1;
						lastIndex = selectedIndex;
						updateScrollDimenstions();
						return true;
					}
				}
			}
			else {
				//TODO implement continuous mode
			}
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
//					orientation = ORIENTATION_TOP_DOWN;
					if( lastIndex < elementsCount - 1 ) {
						firstIndex = lastIndex + 1;
						selectedIndex = firstIndex;
						updateScrollDimenstions();
						return true;
					}
				}
			}
			else { //continuous mode

			}
		}
		else if( orientation == ORIENTATION_BOTTOM_UP ) {
			if( listMode == MODE_PAGE ) {
				if( selectedIndex < lastIndex ) {
					selectedIndex++;
					return true;
				}
				if( selectedIndex == lastIndex ) {
					if( lastIndex < elementsCount - 1 ) {
						firstIndex = lastIndex + 1;
						selectedIndex = firstIndex;
						updateScrollDimenstions();
						return true;
					}
				}
			}
			else { //continuous mode

			}
		}
	}
	return false;
}

void PhonebookListView::updatePageItems() {

	//defines how many slots in the list has been occupied
	int visibleElements = 0;

	// for top down orientation
	if( orientation == ORIENTATION_TOP_DOWN ) {
		//index of requested item is incremented only if payload of returned item is not null
		//if it's null it means that this is favourite label of label connected with showing Capital latters for surnames
		int index = firstIndex, prevIndex = -1;
		lastIndex = firstIndex;
		for( int i=0; i<pageSize; i++ ) {
			ListItem* item = provider->getItem(index, firstIndex, prevIndex, pageSize );

			if( item != nullptr ) {
				addWidget(item);
				items.push_back(item);

				prevIndex = index;
				if( item->getID() >= 0 ) {
					index++;
					this->lastIndex++;
				}
				visibleElements++;
			}
		}
		//after the loop lastIndex is the number of elements not the index of last element that's why -- is used.
		lastIndex--;

		LOG_INFO("Last index: %d", lastIndex );

		//if last element has been displayed but there is still space for elements
		if( (lastIndex == elementsCount - 1) && (visibleElements != pageSize ) && ( firstIndex != 0 )) {

			orientation = ORIENTATION_BOTTOM_UP;

			//clear all elements in the list.
			auto it = items.begin();
			for( uint32_t i=0; i<items.size(); i++ ) {
				ListItem* item = *it;
				removeWidget( item );
				delete item;
				std::advance(it,1);
			}
			items.clear();

			lastIndex = elementsCount - 1;
			firstIndex = lastIndex;
			index = lastIndex;
			prevIndex = -1;
			for( int i=0; i<pageSize; i++ ) {
				ListItem* item = provider->getItem(index, lastIndex, prevIndex, pageSize, pageSize - 1 -i, false );

				if( item != nullptr ) {
					addWidget(item);
					items.push_back(item);

					prevIndex = index;
					if( item->getID() >= 0 ) {
						index--;
						firstIndex--;
					}
				}
			}
			LOG_INFO("BOT-UP first index %d Last index: %d", firstIndex, lastIndex );
		}
	}
	else if( orientation == ORIENTATION_BOTTOM_UP ) {
		//index of requested item is incremented only if payload of returned item is not null
		//if it's null it means that this is favourite label of label connected with showing Capital latters for surnames
		int index = lastIndex, prevIndex = -1;
		firstIndex = lastIndex;
		for( int i=0; i<pageSize; i++ ) {
			ListItem* item = provider->getItem(index, lastIndex, prevIndex, pageSize, pageSize - 1 -i, false );

			if( item != nullptr ) {
				addWidget(item);
				items.push_back(item);

				prevIndex = index;
				if( item->getID() >= 0 ) {
					index--;
					firstIndex--;
				}
				visibleElements++;
			}
		}

		firstIndex++;

		LOG_INFO("first index: %d", firstIndex );

		//if last element has been displayed but there is still space for elements
		if( (lastIndex != elementsCount - 1) && (visibleElements != pageSize ) && ( firstIndex == 0 )) {

			orientation = ORIENTATION_TOP_DOWN;

			//clear all elements in the list.
			auto it = items.begin();
			for( uint32_t i=0; i<items.size(); i++ ) {
				ListItem* item = *it;
				removeWidget( item );
				delete item;
				std::advance(it,1);
			}
			items.clear();

			int index = firstIndex, prevIndex = -1;
			this->lastIndex = firstIndex;
			for( int i=0; i<pageSize; i++ ) {
				ListItem* item = provider->getItem(index, firstIndex, prevIndex, pageSize );

				if( item != nullptr ) {
					addWidget(item);
					items.push_back(item);

					prevIndex = index;
					if( item->getID() >= 0 ) {
						index++;
						this->lastIndex++;
					}
				}
			}
			LOG_INFO("TOP-BOT first index %d Last index: %d", firstIndex, lastIndex );
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
		verticalPosition = widgetArea.h - itemHeight ;

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
