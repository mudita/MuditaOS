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
#include "UiCommonActions.hpp"
#include "ListView.hpp"

using namespace calllog;

#define DEBUG_CALLLOG_DB_ACCESS 0

CalllogModel::CalllogModel(app::Application *app) : DatabaseModel(app)
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
        DBServiceAPI::CalllogGetLimitOffset(application, 0, calllog::settings::pageSize);
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
#if DEBUG_DB_MODEL_DATA == 1
    LOG_DEBUG("Offset: %d, Limit: %d Count:%d", offset, limit, count);
    for (uint32_t i = 0; i < records.get()->size(); ++i) {
        LOG_DEBUG("id: %d, name: %s", records.get()->operator[](i).ID, records.get()->operator[](i).name.c_str());
    }
#endif

    DatabaseModel::updateRecords(std::move(records), offset, limit, count);
    modelIndex = 0;
    list->onProviderDataUpdate();

    return true;
}

gui::ListItem *CalllogModel::getItem(gui::Order order)
{
    auto index = modelIndex;
    if (order == gui::Order::Previous) {
        index = records.size() - 1 - modelIndex;
    }

    std::shared_ptr<CalllogRecord> call = getRecord(index);

    modelIndex++;

    LOG_DEBUG("Model index: %d", index);

    SettingsRecord &settings = application->getSettings();
    if (call.get() == nullptr) {
        // LOG_ERROR("getItem nullptr");
        return nullptr;
    }

    auto item = new gui::CalllogItem(this, !settings.timeFormat12);
    if (item != nullptr) {
        item->setCall(call);
        item->setID(index);
        item->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("activatedCallback");
            std::unique_ptr<gui::SwitchData> data = std::make_unique<calllog::CallLogSwitchData>(*call);
            application->switchWindow(calllog::settings::DetailsWindowStr, std::move(data));
            return true;
        };

        item->inputCallback = [this, item](gui::Item &, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == gui::KeyCode::KEY_LF) {
                LOG_DEBUG("calling");
                return app::call(application, item->getCall().phoneNumber);
            }
            return false;
        };
        return item;
    }
    return nullptr;
}
