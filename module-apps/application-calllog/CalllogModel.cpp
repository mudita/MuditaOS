/*
 * @file CalllogModel.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log Model
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "service-db/api/DBServiceAPI.hpp"
#include "widgets/CalllogItem.hpp"
#include "data/CallLogSwitchData.hpp"
#include "data/CallLogInternals.hpp"
#include "CalllogModel.hpp"

using namespace calllog;

#define DEBUG_CALLLOG_DB_ACCESS 0

CalllogModel::CalllogModel(app::Application *app) : DatabaseModel(app, calllog::settings::pageSize)
{}

void CalllogModel::requestRecordsCount()
{
#if (DEBUG_CALLLOG_DB_ACCESS)
    auto tstamp = xTaskGetTickCount();
#endif
    recordsCount = DBServiceAPI::CalllogGetCount(application);
#if (DEBUG_CALLLOG_DB_ACCESS)
    LOG_INFO("DBServiceAPI::CalllogGetCount %d records %d ms", recordsCount, xTaskGetTickCount() - tstamp);
#endif

    // request first and second page if possible
    if (recordsCount > 0) {
        LOG_INFO("DBServiceAPI::CalllogGetCount CalllogGetLimitOffset");
        DBServiceAPI::CalllogGetLimitOffset(application, 0, pageSize);
        if (recordsCount >= pageSize) {
            DBServiceAPI::CalllogGetLimitOffset(application, pageSize, pageSize);
        }
    }
}

void CalllogModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    DBServiceAPI::CalllogGetLimitOffset(application, offset, limit);
}

bool CalllogModel::updateRecords(std::unique_ptr<std::vector<CalllogRecord>> records,
                                 const uint32_t offset,
                                 const uint32_t limit,
                                 uint32_t count)
{

    LOG_INFO("Offset: %" PRIu32 ", Limit: %" PRIu32 " Count:%" PRIu32 "", offset, limit, count);

    DatabaseModel::updateRecords(std::move(records), offset, limit, count);

    return true;
}

gui::ListItem *CalllogModel::getItem(
    int index, int firstElement, int prevElement, uint32_t count, int remaining, bool topDown)
{
    std::shared_ptr<CalllogRecord> call = getRecord(index);
    SettingsRecord &settings            = application->getSettings();
    if (call.get() == nullptr) {
        // LOG_ERROR("getItem nullptr");
        return nullptr;
    }
    auto item = new gui::CalllogItem(this, !settings.timeFormat12);
    if (item != nullptr) {
        item->setCall(call);
        item->setID(index);
        item->activatedCallback = [=](gui::Item &item) { // TODO: alek: this is not the best place for this
            LOG_INFO("activatedCallback");
            std::unique_ptr<gui::SwitchData> data = std::make_unique<calllog::CallLogSwitchData>(*call);
            application->switchWindow(calllog::settings::DetailsWindowStr, std::move(data));
            return true;
        };
        return item;
    }

    return nullptr;
}
