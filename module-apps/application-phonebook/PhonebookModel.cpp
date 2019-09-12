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


PhonebookModel::PhonebookModel(  app::Application* app) : DatabaseModel(app, 7){

}

PhonebookModel::~PhonebookModel() {
	// TODO Auto-generated destructor stub
}

void PhonebookModel::requestRecordsCount() {

	uint32_t start = xTaskGetTickCount();
	favouriteCount = DBServiceAPI::ContactGetCount(application, true );
	uint32_t stop = xTaskGetTickCount();
	LOG_INFO("DBServiceAPI::PhonebookGetCount %d records %d ms", favouriteCount, stop-start);

	start = xTaskGetTickCount();
	recordsCount = DBServiceAPI::ContactGetCount(application);
	stop = xTaskGetTickCount();
	LOG_INFO("DBServiceAPI::PhonebookGetCount %d records %d ms", recordsCount, stop-start);

	//if there are favorite records request as much as possible
	uint32_t pageSize = this->pageSize;
	uint32_t request = 0;
	uint32_t requestedTotal = 0;
	if( favouriteCount ) {
		request = favouriteCount>pageSize?pageSize:favouriteCount;
		requestedTotal += request;
		DBServiceAPI::ContactGetLimitOffset( application, 0, request, true );
		if( favouriteCount > pageSize ){
			request = favouriteCount-pageSize>pageSize?pageSize:favouriteCount-pageSize;
			DBServiceAPI::ContactGetLimitOffset( application, pageSize, request, true );
			requestedTotal += request;
		}
	}

	//if there is still space for normal request remaining part of normal contacts
	if( requestedTotal < 2* pageSize ) {
		request = pageSize;
		DBServiceAPI::ContactGetLimitOffset( application, 0,request, false );
	}
}

void PhonebookModel::requestRecords( const uint32_t offset, const uint32_t limit ) {
	//if it's needed to request only favorite
	if( offset < favouriteCount - limit ){
		DBServiceAPI::ContactGetLimitOffset( application, offset, limit, true );
	}
	//it's needed to request favorite contacts but also some elements from normal contacts
	else if( (offset < favouriteCount) && (offset + limit >= favouriteCount) ) {
		DBServiceAPI::ContactGetLimitOffset( application, offset, favouriteCount - offset, true );
		DBServiceAPI::ContactGetLimitOffset( application, 0, limit - favouriteCount + offset, false );
	}
	//request normal contacts
	else {
		DBServiceAPI::ContactGetLimitOffset( application, offset - favouriteCount, limit, false );
	}
}

bool PhonebookModel::updateRecords( std::unique_ptr<std::vector<ContactRecord>> records, const uint32_t offset, const uint32_t limit, uint32_t count, bool favourite  ) {

	LOG_INFO("Offset: %d, Limit: %d Count:%d", favourite?offset:offset+favouriteCount, limit, count);
	for( uint32_t i=0; i<records.get()->size(); ++i ) {
		LOG_INFO("id: %d, name: %s surname: %s favourite: %s",
			records.get()->operator [](i).dbID,
			records.get()->operator [](i).primaryName.c_str(),
			records.get()->operator [](i).alternativeName.c_str(),
			(records.get()->operator [](i).isOnFavourites?"TRUE":"FALSE"));
	}
	if( favourite )
		DatabaseModel::updateRecords( std::move(records), offset, limit, count );
	else
		DatabaseModel::updateRecords( std::move(records), offset+favouriteCount, limit, count );


	return true;
}

gui::ListItem* PhonebookModel::getItem( int index, int firstElement,  int prevIndex, uint32_t count ) {
	std::shared_ptr<ContactRecord> contact = getRecord( index );

	if( contact == nullptr )
		return nullptr;


	//return item from favorite part of contacts
	if( static_cast<uint32_t>(index) < favouriteCount ) {
		gui::PhonebookItem* item = new gui::PhonebookItem(this);
		if( (index == firstElement) && (index != prevIndex) ) {
			item->setValue("Favourite");
		}
		else {
			item->setContact(contact);
			item->setID( index );
		}
		return item;
	}
	else {
		gui::PhonebookItem* item = new gui::PhonebookItem(this);
		//on top the page or if element next after last favourite contact is requested
		if( (index == firstElement) ||
			(static_cast<uint32_t>(index) == favouriteCount) ) {

			item->setValue( contact->alternativeName.substr(0,1));
		}
		else {
			item->setContact(contact);
			item->setID( index );
		}
		return item;
	}
	return nullptr;
}
int PhonebookModel::getItemCount() {
	return recordsCount + favouriteCount;
}

