/*
 * @file DatabaseModel.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details This class is a template to create specialized instances of
 * buffers that hold database records from given table.
 */
#ifndef MODULE_APPS_DATABASEMODEL_HPP_
#define MODULE_APPS_DATABASEMODEL_HPP_

#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>

#include "Application.hpp"

namespace app {

/*
 *
 */
template <class T> class DatabaseModel {
protected:
	/**
	 * Number of elements requested from database in single message
	 */
	int pageSize;
	/**
	 * Pointer to application that owns the model
	 */
	Application* application;
	/**
	 * Total number of records in database
	 */
	int recordsCount;
	/**
	 * Index of first element in current page
	 */
	int firstIndex;
	//vector that holds records received from database - first last records, than previous records and finally next records
	std::vector<std::shared_ptr<T>> records;
public:
	virtual void destroyRecord( T* record ) {
		LOG_FATAL("!!! You should provide custom destroy function ");
	}

	virtual void requestRecordsCount() {
	}

	virtual bool updateRecords( std::unique_ptr<std::vector<T>> records, const uint32_t offset, const uint32_t limit, uint32_t count ) {
		return true;
	}

	virtual void movePrevWindow() {

//		uint32_t currentPage = firstIndex / pageSize;
//		//check if it is possible to move to previous window
//		if( currentPage <= 0 )
//			return;
//
//		//erase records from next window
//		for( int i=0; i<pageSize; i++ )
//			if( records[i+2*pageSize]) {
//				destroyRecord(records[i+2*pageSize]);
//				records[i+2*pageSize] = nullptr;
//			}
//
//		//moves current records to next records and prev records to current records
//		std::copy_backward(std::begin(records), std::begin(records) + pageSize*2, std::end(records) );
//
//		//clear first window to nullptr
//		std::fill( records.begin(), records.begin()+pageSize, nullptr );
//
//		//lock the application for keyboard input
//		application->blockEvents(true);
//
//		//request records for next window
//		uint32_t prevPage = currentPage - 2;
//		if( prevPage >= 0) {
//			requestRecords(pageSize*prevPage, pageSize );
//		}
//		firstIndex -= pageSize;
//		if( firstIndex < 0 )
//			firstIndex = 0;
	}

	virtual void moveNextWindow() {

//		uint32_t currentPage = firstIndex / pageSize;
//		uint32_t pageCount = (recordsCount % pageSize == 0) ? recordsCount/pageSize : (recordsCount/pageSize)  + 1;
//		//check if it is possible to move to next window
//		if( currentPage >= pageCount )
//			return;
//
//		//erase first window with indicies from 0 up to pageSize -1
//		for( int i=0; i<pageSize; i++ )
//			if( records[i] )
//				destroyRecord(records[i]);
//
//		//move current and next window so current window becomes prev window and next becomes current.
//		std::copy_n(std::begin(records)+pageSize, 2*pageSize, begin(records));
//
//		//clear last window to nullptr
//		std::fill( records.begin()+2*pageSize, records.end(), nullptr );
//
//		//lock the application for keyboard input
//		application->blockEvents(true);
//
//		//request records for next window
//		uint32_t nextPage = currentPage + 2;
//		if( nextPage < pageCount )
//			requestRecords(pageSize*nextPage, pageSize );
//		firstIndex += pageSize;
//		if( firstIndex > recordsCount )
//			firstIndex = recordsCount/pageCount;
	}

	virtual void requestRecords( const uint32_t offset, const uint32_t limit ) {
	}

	bool setRecordsCount( int count ) {

		clear();
		recordsCount = count;

		//if there are any records request blocks of data from database using recordsLocalCount and requestSize.
		if( recordsCount ) {

			//TODO handle validation for firstIndex when number of records has decreased.
			requestRecords(firstIndex, pageSize );
			if( firstIndex + pageSize < recordsCount )
				requestRecords(firstIndex+pageSize, pageSize );
			return true;
		}
		return false;
	}

	void setPageSize( int size ) {
		clear();
		pageSize = size;
		firstIndex = 0;

		records.resize(3*pageSize);
		std::fill( records.begin(), records.end(), nullptr );
	}

	DatabaseModel( Application* app ) :
		pageSize{3},
		application{app},
		recordsCount{ -1 },
		firstIndex{ 0 } {

		records.resize(pageSize);
		std::fill( records.begin(), records.end(), nullptr );
	}

	virtual ~DatabaseModel() {
		LOG_INFO("~DatabaseModel");
//		for( auto& r : records )
//			destroyRecord(r);
//		records.clear();
	}

	virtual void clear() {
//		for( auto& r : records ) {
//			destroyRecord(r);
//		}
//		std::fill( records.begin(), records.end(), nullptr );
//		firstIndex = 0;
//		recordsCount = -1;
	}

	int getRecordsCount() { return recordsCount; };

	int getPage( uint32_t index ) {
		return  (index % pageSize == 0) ? index/pageSize : (index/pageSize)  + 1;
	}

	T* getRecord( int index ) {

		//if index is greater than number of records or smaller than 0
		if( (index<0) || (index>recordsCount-1)){
			return nullptr;
		}
		//if index is in current window
		if( (index >= firstIndex) && ( index < firstIndex + pageSize)) {
			return records[pageSize + index-firstIndex];
		}
		//if index is in next window
		if( (index >= firstIndex + pageSize) && ( index < firstIndex+2*pageSize) ) {
			T* record = records[pageSize + index-firstIndex];
			moveNextWindow();
			return record;
		}
		//if index is in previous window
		if( (index >= firstIndex - pageSize) && ( index < firstIndex) ) {
			T* record = records[index+ pageSize-firstIndex];
			movePrevWindow();
			return record;
		}
		//if index is not in prev, current or next window
		else if( (index < firstIndex - pageSize) || (index>firstIndex + 2*pageSize) ){
			int newCurrentPage = index/pageSize;
			clear();
			firstIndex = pageSize * newCurrentPage;
			//request current page
			requestRecords(firstIndex, pageSize );
			//if possible request previous page;
			if( firstIndex - 1 > 0 )
				requestRecords(firstIndex - pageSize, pageSize);
			//if possible request next window
			if( firstIndex*(pageSize + 1) <= recordsCount - 1 )
				requestRecords(firstIndex+pageSize, pageSize);

			return nullptr;
		}
		return nullptr;
	}
};

} /* namespace app */

#endif /* MODULE_APPS_DATABASEMODEL_HPP_ */
