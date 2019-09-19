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
	int pageSize = 1;
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

	virtual void requestRecordsCount() {
	}

	virtual bool updateRecords( std::unique_ptr<std::vector<T>> records, const uint32_t offset, const uint32_t limit, uint32_t count ) {
		//calculate for which page is received data using offset value

		int page = getPage(offset);
		int currentPage = firstIndex / pageSize;

		if( (page >= currentPage - 1) && ( page <= currentPage + 1)){

			//store new records
			for( unsigned int i=0; i<count; i++ ) {
				auto ii = i+(pageSize*(page-currentPage) + pageSize)+offset% pageSize;
				this->records[ii] = nullptr;
				this->records[i+(pageSize*(page-currentPage) + pageSize)+offset% pageSize] = std::make_shared<T>( records.get()->operator [](i) );
				//this->records[i+(pageSize*(page-currentPage) + pageSize)+offset% limit] = std::shared_ptr<T>( new T(records.get()->operator [](i)) );
			}

			//return true (for refreshing window) only if current page was modified.
			if( page == currentPage )
				return true;
			return false;
		}
		else {
			//alarms are out of current scope
			LOG_ERROR("Received records out of scope");
			return false;
		}
		return false;
	}

	virtual void movePrevWindow() {

		uint32_t currentPage = firstIndex / pageSize;
		//check if it is possible to move to previous window
		if( currentPage <= 0 )
			return;

		//erase records from next window
		for( int i=0; i<pageSize; i++ )
			if( records[i+2*pageSize]) {
				records[i+2*pageSize] = nullptr;
			}

		//moves current records to next records and prev records to current records
		for( int i=3*pageSize-1; i>=pageSize; --i )
			records[i] = records[i-pageSize];

		//clear first window to nullptr
		for( int i = 0; i<pageSize; ++i )
			records[i] = nullptr;

		//lock the application for keyboard input
		application->blockEvents(true);

		//request records for next window
		int prevPage = currentPage - 2;
		if( prevPage >= 0) {
			requestRecords(pageSize*prevPage, pageSize );
		}
		firstIndex -= pageSize;
		if( firstIndex < 0 )
			firstIndex = 0;
	}

	virtual void moveNextWindow() {

		uint32_t currentPage = firstIndex / pageSize;
		uint32_t pageCount = (recordsCount % pageSize == 0) ? recordsCount/pageSize : (recordsCount/pageSize)  + 1;
		//check if it is possible to move to next window
		if( currentPage >= pageCount )
			return;

		//erase firswt window with indicies from 0 up to pageSize -1
		for( int i=0; i<pageSize; i++ )
			if( records[i] )
				records[i] = nullptr;

		//move current and next window so current window becomes prev window and next becomes current.
		for( int i=0; i<2*pageSize; ++i )
			records[i] = records[i+pageSize];

		//clear last window to nullptr
		for( int i= 2*pageSize; i<3*pageSize; ++i )
			records[i] = nullptr;

		//lock the application for keyboard input
		application->blockEvents(true);

		//request records for next window
		uint32_t nextPage = currentPage + 2;
		if( nextPage < pageCount )
			requestRecords(pageSize*nextPage, pageSize );
		firstIndex += pageSize;
		if( firstIndex > recordsCount )
			firstIndex = recordsCount/pageCount;
	}

	virtual void requestRecords( const uint32_t offset, const uint32_t limit ) = 0;

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

	DatabaseModel( Application* app, int pageSize ) :
		pageSize{pageSize},
		application{app},
		recordsCount{ -1 },
		firstIndex{ 0 } {

		records.resize( pageSize*3 );
		std::fill( records.begin(), records.end(), nullptr );
	}

	virtual ~DatabaseModel() {
		clear();
	}

	virtual void clear() {

		for( int i=0; i< pageSize*3; i++ )
			records[i] = nullptr;
		records.clear();
		firstIndex = 0;
		recordsCount = -1;
	}

	int getRecordsCount() {
		return recordsCount;
	}

	int getPage( uint32_t index ) {
		return  (index % pageSize == 0) ? index/pageSize : (index/pageSize);
	}

	/**
	 * @brief Returns database object under specified index. Performs forward and backward buffering.
	 * @index Index of the database element. Starts from 0 ( database objects have lowest ID equal to 1. )
	 * @download Flag that controls whether model should download objects from database when requested object is
	 * outside visible window.
	 */
	std::shared_ptr<T> getRecord( int index, bool download = true ) {

		//if index is greater than number of records or smaller than 0
		if( (index<0) || (index>recordsCount-1)){
			return nullptr;
		}
		//if index is in current window
		if( (index >= firstIndex) && ( index < firstIndex + pageSize)) {
			return records[pageSize + index-firstIndex];
		}
		//if download is false check if requested element is in any of the windows
		if( (download == false ) && (index >= firstIndex - pageSize ) && ( index < firstIndex + 2*pageSize)) {
			return records[pageSize + index-firstIndex];
		}
		//if index is in next window
		if( download && (index >= firstIndex + pageSize) && ( index < firstIndex+2*pageSize) ) {
			std::shared_ptr<T> record = records[pageSize + index - firstIndex];
			moveNextWindow();
			return record;
		}
		//if index is in previous window
		if( download && (index >= firstIndex - pageSize) && ( index < firstIndex) ) {
			std::shared_ptr<T> record = records[index+ pageSize-firstIndex];
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
