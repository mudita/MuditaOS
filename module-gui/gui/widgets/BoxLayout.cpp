/*
 * BoxLayout.cpp
 *
 *  Created on: 16 maj 2019
 *      Author: robert
 */

#include "BoxLayout.hpp"

namespace gui {

BoxLayout::BoxLayout() : Rect() {

}
BoxLayout::BoxLayout( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h) :
	Rect ( parent, x, y, w, h ){
}

BoxLayout::~BoxLayout() {
}

void BoxLayout::resizeItems() {
}

void BoxLayout::setPosition( const short& x, const short& y ) {
	Rect::setPosition( x, y );
}
void BoxLayout::setSize( const short& w, const short& h ) {
	Rect::setSize( w, h );

	resizeItems();
}
bool BoxLayout::addWidget( Item* item ) {
	bool ret = Rect::addWidget( item );

	resizeItems();

	return ret;
}
bool BoxLayout::removeWidget( Item* item ) {
	bool ret = Rect::removeWidget( item );

	resizeItems();

	return ret;
}
std::list<DrawCommand*> BoxLayout::buildDrawList() {
	return Rect::buildDrawList();
}


HBox::HBox() : BoxLayout() {

}
HBox::HBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h) :
	BoxLayout( parent, x, y, w, h ) {
}

void HBox::resizeItems() {

	if( children.empty())
		return;

	uint32_t freeSpace = widgetArea.w;
	int16_t newX = 0;
	int16_t newY = 0;
	uint16_t newH = widgetArea.h;
	uint16_t maxWidthValue;
	uint16_t newW;

	//create list of elements to be modified. Each element is placed in the BoxElement structure;
	//width of all elements are set to 0
	std::list<BoxElement> boxElements;
	for( Item* item : children ) {
		boxElements.push_back( BoxElement(item) );
		item->widgetArea.w = 0;
	}

	int32_t elementsToChange = children.size();
	while( freeSpace > 0 ) {
		newX = 0;

		uint16_t expandValue = freeSpace / elementsToChange;

		//for case when freeSpace is greater than 0 but smaller than elementsToChange
		//expand value is set to 1
		if( expandValue == 0 )
			expandValue = 1;

		std::list<BoxElement>::iterator i = boxElements.begin();

		while ((i != boxElements.end()) && (freeSpace > 0))
		{
			//element was already updated in previous run and no further modification is possible or required
			if( (*i).noUpdate ) {
				(*i).item->setPosition( newX, newY );
				newX += (*i).item->widgetArea.w;
				++i;
				continue;
			}
			else if( (*i).item->widgetArea.w == (*i).item->getMaxWidth())
			{
				newH = (*i).item->widgetArea.h;
				newW = (*i).item->widgetArea.w;
				newX += newW;

				freeSpace -= newW;

				(*i).item->setPosition( newX, newY );
				(*i).item->setSize( newW, newH );
				(*i).noUpdate = true;
				elementsToChange--;
			}
			else
			{
				maxWidthValue = (*i).item->getMaxWidth();
				newW = (*i).item->widgetArea.w + expandValue;

				if( newW > maxWidthValue ) {
					newW = maxWidthValue;
					elementsToChange--;
					(*i).noUpdate = true;
				}

				freeSpace -= (newW - (*i).item->widgetArea.w);
				(*i).item->setPosition( newX, newY );
				(*i).item->setSize( newW, newH );

				newX += newW;
			}

			++i;
		}

		if(elementsToChange <= 0 )
			break;
	}

//	for( Item* item : children )
//		std::cout<<"x: "<< item->widgetArea.x<<" y: "<< item->widgetArea.y<<
//		" w: "<< item->widgetArea.w<<" h: "<< item->widgetArea.h<<std::endl;
//
//	std::cout<<std::endl;
	Rect::updateDrawArea();
}


VBox::VBox() : BoxLayout() {

}
VBox::VBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h) :
	BoxLayout( parent, x, y, w, h ) {
}

void VBox::resizeItems() {

	if( children.empty())
		return;

	uint32_t freeSpace = widgetArea.h;
	int16_t newX = 0;
	int16_t newY = 0;
	uint16_t newH;
	uint16_t maxHeightValue;
	uint16_t newW = widgetArea.w;

	//create list of elements to be modified. Each element is placed in the BoxElement structure;
	//width of all elements are set to 0
	std::list<BoxElement> boxElements;
	for( Item* item : children ) {
		boxElements.push_back( BoxElement(item) );
		item->widgetArea.h = 0;
	}

	uint32_t elementsToChange = children.size();
	while( freeSpace > 0 ) {
		newY = 0;
		if( freeSpace < elementsToChange ) {
			break;
		}
		uint16_t expandValue = freeSpace / elementsToChange;

		//for case when freeSpace is greater than 0 but smaller than elementsToChange
		//expand value is set to 1
		if( expandValue == 0 )
			expandValue = 1;

		std::list<BoxElement>::iterator i = boxElements.begin();

		while ((i != boxElements.end()) && (freeSpace > 0))
		{
			//element was already updated in previous run and no further modification is possible or required
			if( (*i).noUpdate ) {
				(*i).item->setPosition( newX, newY );
				newY += (*i).item->widgetArea.h;
				++i;
				continue;
			}
			else if( (*i).item->widgetArea.h == (*i).item->getMaxHeight())
			{
				newH = (*i).item->widgetArea.h;
				newW = (*i).item->widgetArea.w;
				newY += newH;

				freeSpace -= newH;

				(*i).item->setPosition( newX, newY );
				(*i).item->setSize( newW, newH );
				(*i).noUpdate = true;
				elementsToChange--;
			}
			else
			{
				maxHeightValue = (*i).item->getMaxHeight();
				newH = (*i).item->widgetArea.h + expandValue;

				if( newH > maxHeightValue ) {
					newH = maxHeightValue;
					elementsToChange--;
					(*i).noUpdate = true;
				}

				freeSpace -= (newH - (*i).item->widgetArea.h);
				(*i).item->setPosition( newX, newY );
				(*i).item->setSize( newW, newH );

				newY += newH;
			}

			++i;
		}

		if(elementsToChange == 0 )
			break;
	}

//	for( Item* item : children )
//		std::cout<<"x: "<< item->widgetArea.x<<" y: "<< item->widgetArea.y<<
//		" w: "<< item->widgetArea.w<<" h: "<< item->widgetArea.h<<std::endl;
//
//	std::cout<<std::endl;
	Rect::updateDrawArea();
}
} /* namespace gui */
