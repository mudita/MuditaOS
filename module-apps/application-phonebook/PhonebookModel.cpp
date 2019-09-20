/*
 * @file PhonebookModel.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 10 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "PhonebookModel.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include "widgets/PhonebookItem.hpp"


PhonebookModel::PhonebookModel(  app::Application* app) : DatabaseModel(app, 14){

}

PhonebookModel::~PhonebookModel() {
	// TODO Auto-generated destructor stub
}

void PhonebookModel::requestRecordsCount() {

//	uint32_t start = xTaskGetTickCount();
	favouriteCount = DBServiceAPI::ContactGetCount(application, true );
//	uint32_t stop = xTaskGetTickCount();
//	LOG_INFO("DBServiceAPI::PhonebookGetCount %d records %d ms", favouriteCount, stop-start);

//	start = xTaskGetTickCount();
	recordsCount = DBServiceAPI::ContactGetCount(application)  +favouriteCount;
//	stop = xTaskGetTickCount();
//	LOG_INFO("DBServiceAPI::PhonebookGetCount %d records %d ms", recordsCount, stop-start);

	//if there are favorite records request as much as possible
	uint32_t pageSize = this->pageSize;

	requestRecords( 0, pageSize );
	requestRecords( pageSize, pageSize );
}

void PhonebookModel::requestRecords( const uint32_t offset, const uint32_t limit ) {

	if( offset < favouriteCount ){
		//if it's needed to request only favorite
		if( offset + limit < favouriteCount ) {
			DBServiceAPI::ContactGetLimitOffset( application, offset, limit, true );
		}
		else {
			uint32_t count = favouriteCount-offset;
			DBServiceAPI::ContactGetLimitOffset( application, offset, count, true );
			DBServiceAPI::ContactGetLimitOffset( application, 0, limit - count, false);
		}
	}
	//request normal contacts
	else {
		DBServiceAPI::ContactGetLimitOffset( application, offset - favouriteCount, limit, false );
	}
}

bool PhonebookModel::updateRecords( std::unique_ptr<std::vector<ContactRecord>> records, const uint32_t offset, const uint32_t limit, uint32_t count, bool favourite  ) {

	LOG_INFO("Offset: %d, Limit: %d Count:%d", favourite?offset:offset+favouriteCount, limit, count);

	if( favourite )
		DatabaseModel::updateRecords( std::move(records), offset, limit, count );
	else
		DatabaseModel::updateRecords( std::move(records), offset+favouriteCount, limit, count );

	return true;
}

gui::ListItem* PhonebookModel::getItem( int index, int firstElement,  int prevIndex, uint32_t count, int remaining, bool topDown ) {

	bool download = false;
	if( index > firstIndex + pageSize / 2 )
		download = true;
	if( index < firstIndex - pageSize / 2 )
		download = true;
	std::shared_ptr<ContactRecord> contact = getRecord( index, download );

	if( contact == nullptr )
		return nullptr;

	if( topDown ) {
		//return item from favorite part of contacts
		if( static_cast<uint32_t>(index) < favouriteCount ) {
			gui::PhonebookItem* item = new gui::PhonebookItem(this);

			if( (index == firstElement) && (index != prevIndex) ) {
				item->setValue("Favourite");
			}
			else {
				item->markFavourite(true);
				item->setContact(contact);
				item->setID( index );
			}
			return item;
		}
		else {
			gui::PhonebookItem* item = new gui::PhonebookItem(this);
			//on top the page or if element next after last favourite contact is requested
			if( ((index == firstElement) ||	(static_cast<uint32_t>(index) == favouriteCount)) &&
					(index!=prevIndex) ) {

				item->setValue( contact->alternativeName.substr(0,1));
			}
			else {
				std::shared_ptr<ContactRecord> prevContact = getRecord( prevIndex, false  );
				if( contact->alternativeName.substr(0,1) == prevContact->alternativeName.substr(0,1)) {
					item->setContact(contact);
					item->setID( index );
				}
				else {
					item->setValue( contact->alternativeName.substr(0,1));
				}
			}
			return item;
		}
	}
	else{
		LOG_INFO("BOT-UP index: %d first: %d prev: %d count: %d rem: %d", index, firstElement, prevIndex, count, remaining );

		if( static_cast<uint32_t>(index) < favouriteCount -1 ) {
			gui::PhonebookItem* item = new gui::PhonebookItem(this);
			if( remaining == 0 ) {
				item->setValue("Favourite");
			}
			else {
				item->markFavourite(true);
				item->setContact(contact);
				item->setID( index );
			}
			return item;
		}
		else {
			gui::PhonebookItem* item = new gui::PhonebookItem(this);

			//leaving normal contacts list and entering favourite area but charatcer is already placed
			if( (static_cast<uint32_t>(index) == favouriteCount -1 ) && (index == prevIndex)) {
				item->markFavourite(true);
				item->setContact(contact);
				item->setID( index );
			}
			//leaving normal contacts list and entering favourite area - return character
			if( (static_cast<uint32_t>(index) == favouriteCount - 1) && (index != prevIndex)) {
				item->setValue( contact->alternativeName.substr(0,1));
			}
			else {
				std::shared_ptr<ContactRecord> prevContact = getRecord( prevIndex, false  );
				if( remaining == 0 ) {
					//previous element has the same first character of alternative name so display first character
					if( index == prevIndex ){
						item->setContact(contact);
						item->setID( index );
					}
					else {
						item->setValue( prevContact->alternativeName.substr(0,1));
					}
				}
				else
					if(
					( ( index == firstElement ) ||
					( index == prevIndex ) ||
							( contact->alternativeName.substr(0,1) == prevContact->alternativeName.substr(0,1)) )){
					item->setContact(contact);
					item->setID( index );
				}
				else {
					item->setValue( prevContact->alternativeName.substr(0,1));
				}
			}
			return item;
		}
	}
	return nullptr;
}

int PhonebookModel::getItemCount() {
	return recordsCount;
}

