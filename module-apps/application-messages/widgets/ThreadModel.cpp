/*
 * ThreadModel.cpp
 *
 *  Created on: 15 lis 2019
 *      Author: kuba
 */


#include "ThreadModel.hpp"
#include "ThreadItem.hpp"

#include "Application.hpp"

#include "../MessagesStyle.hpp"

#include "service-db/api/DBServiceAPI.hpp"
ThreadModel::ThreadModel( app::Application* app) : DatabaseModel( app, messages::threadsPageSize)
{

}

void ThreadModel::requestRecordsCount( void )
{
	recordsCount = DBServiceAPI::ThreadGetCount(application);

	if( recordsCount > 0)
	{

		DBServiceAPI::ThreadGetLimitOffset(application, 0, messages::threadsPageSize);
		if( recordsCount >= messages::threadsPageSize)
		{
			DBServiceAPI::ThreadGetLimitOffset(application, messages::threadsPageSize, messages::threadsPageSize);
		}
	}
}
bool ThreadModel::updateRecords(std::unique_ptr<std::vector<ThreadRecord>> records, const uint32_t offset, const uint32_t limit, uint32_t count )
{
	auto ret = DatabaseModel::updateRecords(std::move(records), offset, limit, count);

	return ret;
}

void ThreadModel::requestRecords(uint32_t offset, uint32_t limit)
{
	DBServiceAPI::ThreadGetLimitOffset(application, offset, limit);
}

gui::ListItem* ThreadModel::getItem(int index, int fistElement, int prevElement, uint32_t limit,int remaining, bool topDown )
{
	std::shared_ptr<ThreadRecord> thread = getRecord( index );

	if( thread.get() == nullptr)
	{
		return nullptr;
	}

	auto item = new gui::ThreadItem(this);
	if( item != nullptr )
	{
		item->setThreadItem(thread);
		item->activatedCallback = [=] (gui::Item& item){
			LOG_INFO("ThreadItem ActivatedCallback");
			return true;
		};
		return item;
	}
	return nullptr;
}
