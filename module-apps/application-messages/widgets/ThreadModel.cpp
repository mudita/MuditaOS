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
#include "../data/SMSdata.hpp"
#include "../windows/ThreadViewWindow.hpp" // for name of window

#include "service-db/api/DBServiceAPI.hpp"
ThreadModel::ThreadModel(app::Application *app) :
		DatabaseModel(app, messages::threads::pageSize) {

}

void ThreadModel::requestRecordsCount(void) {
	recordsCount = DBServiceAPI::ThreadGetCount(application);

	if (recordsCount > 0) {

		DBServiceAPI::ThreadGetLimitOffset(application, 0,
				messages::threads::pageSize);
		if (recordsCount >= messages::threads::pageSize) {
			DBServiceAPI::ThreadGetLimitOffset(application,
					messages::threads::pageSize, messages::threads::pageSize);
		}
	}
}
bool ThreadModel::updateRecords(
		std::unique_ptr<std::vector<ThreadRecord>> records,
		const uint32_t offset, const uint32_t limit, uint32_t count) {
	DatabaseModel::updateRecords(std::move(records), offset, limit, count);

	return true;
}

void ThreadModel::requestRecords(uint32_t offset, uint32_t limit) {
	DBServiceAPI::ThreadGetLimitOffset(application, offset, limit);
}

gui::ListItem* ThreadModel::getItem(int index, int fistElement, int prevElement,
		uint32_t limit, int remaining, bool topDown) {
	std::shared_ptr<ThreadRecord> thread = getRecord(index);

	if (thread.get() == nullptr) {
		return nullptr;
	}

	auto item = new gui::ThreadItem(this);
	if (item != nullptr) {
		item->setThreadItem(thread);
		item->setID(index);
        item->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("ThreadItem ActivatedCallback");
            if (application)
            {
                application->switchWindow(gui::name::window::thread_view, std::make_unique<SMSThreadData>(thread));
            }
            else
            {
                LOG_ERROR("No application!");
            }
            return true;
        };
        return item;
    }
	return nullptr;
}
