// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeUnitsModel.hpp"

#include <application-bell-settings/data/BellSettingsStyle.hpp>

#include <apps-common/widgets/TimeSetFmtSpinner.hpp>
#include <gui/widgets/ListViewEngine.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/Text.hpp>
#include <service-time/Constants.hpp>
#include <service-time/service-time/TimeMessage.hpp>

#include <ctime>

namespace app::bell_settings
{
    TimeUnitsModel::TimeUnitsModel(app::Application *app) : application(app)
    {}

    TimeUnitsModel::~TimeUnitsModel()
    {
        clearData();
    }

    auto TimeUnitsModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto TimeUnitsModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    auto TimeUnitsModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    void TimeUnitsModel::createData()
    {
        timeSetWidget = new gui::TimeSetSpinnerListItem(
            0U, 0U,
            gui::bell_settings_style::time_set_spinner_list_item::w,
            gui::bell_settings_style::time_set_spinner_list_item::h,
            utils::translate("app_bell_settings_time_units_time_message"));
        internalData.push_back(timeSetWidget);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    void TimeUnitsModel::clearData()
    {
        eraseInternalData();
    }

    void TimeUnitsModel::saveData()
    {
        std::time_t now         = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct std::tm *newTime = std::localtime(&now);
        newTime->tm_hour        = timeSetWidget->timeSetFmtSpinner->getHour();
        newTime->tm_min         = timeSetWidget->timeSetFmtSpinner->getMinute();
        LOG_INFO("Setting new time: %d:%d", newTime->tm_hour, newTime->tm_min);
        sendRtcUpdateTimeMessage(std::mktime(newTime));
    }

    void TimeUnitsModel::loadData()
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        timeSetWidget->timeSetFmtSpinner->setHour(std::localtime(&now)->tm_hour);
        timeSetWidget->timeSetFmtSpinner->setMinute(std::localtime(&now)->tm_min);
    }

    auto TimeUnitsModel::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    void TimeUnitsModel::sendRtcUpdateTimeMessage(time_t newTime)
    {
        auto msg = std::make_shared<stm::message::TimeChangeRequestMessage>(newTime);
        application->bus.sendUnicast(std::move(msg), service::name::service_time);
    }

} // namespace app::bell_settings
