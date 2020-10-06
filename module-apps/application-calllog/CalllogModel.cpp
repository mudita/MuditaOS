/*
 * @file CalllogModel.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log Model
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CalllogModel.hpp"
#include "InputEvent.hpp"
#include "ListView.hpp"
#include "UiCommonActions.hpp"
#include "data/CallLogInternals.hpp"
#include "data/CallLogSwitchData.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "widgets/CalllogItem.hpp"

using namespace calllog;

CalllogModel::CalllogModel(app::Application *app) : DatabaseModel(app)
{}

unsigned int CalllogModel::requestRecordsCount()
{
    recordsCount = DBServiceAPI::CalllogGetCount(application);
    return recordsCount;
}

void CalllogModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    DBServiceAPI::CalllogGetLimitOffset(application, offset, limit);
}

bool CalllogModel::updateRecords(std::unique_ptr<std::vector<CalllogRecord>> records)
{
#if DEBUG_DB_MODEL_DATA == 1
    LOG_DEBUG("Offset: %" PRIu32 ", Limit: %" PRIu32 " Count: %" PRIu32 "", offset, limit, count);
    for (uint32_t i = 0; i < records.get()->size(); ++i) {
        LOG_DEBUG(
            "id: %" PRIu32 ", name: %s", records.get()->operator[](i).ID, records.get()->operator[](i).name.c_str());
    }
#endif

    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();

    return true;
}

unsigned int CalllogModel::getMinimalItemHeight() const
{

    return gui::clItemStyle::h;
}

gui::ListItem *CalllogModel::getItem(gui::Order order)
{

    std::shared_ptr<CalllogRecord> call = getRecord(order);

    SettingsRecord &settings = application->getSettings();
    if (call.get() == nullptr) {
        return nullptr;
    }

    auto item = new gui::CalllogItem(this, !settings.timeFormat12);

    item->setCall(call);
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
