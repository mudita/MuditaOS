// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmWindowModel.hpp"

#include <application-bell-alarm/data/BellAlarmStyle.hpp>

#include <apps-common/widgets/TimeSetFmtSpinner.hpp>
#include <gui/widgets/ListViewEngine.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/Text.hpp>
#include <service-time/Constants.hpp>
#include <service-time/service-time/TimeMessage.hpp>

#include <ctime>

namespace app::bell_alarm
{
    BellAlarmWindowModel::BellAlarmWindowModel(app::Application *app) : application(app)
    {}

    BellAlarmWindowModel::~BellAlarmWindowModel()
    {
        clearData();
    }

    auto BellAlarmWindowModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto BellAlarmWindowModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    auto BellAlarmWindowModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    void BellAlarmWindowModel::createData()
    {
        timeSetWidget = new gui::TimeSetSpinnerListItem(
            0U, 0U,
            gui::bell_alarm_style::time_set_spinner_list_item::w,
            gui::bell_alarm_style::time_set_spinner_list_item::h,
            utils::translate("app_bell_settings_time_units_time_message"));
        internalData.push_back(timeSetWidget);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    void BellAlarmWindowModel::clearData()
    {
        eraseInternalData();
    }

    void BellAlarmWindowModel::saveData()
    {
        std::time_t now         = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct std::tm *newTime = std::localtime(&now);
        newTime->tm_hour        = timeSetWidget->timeSetFmtSpinner->getHour();
        newTime->tm_min         = timeSetWidget->timeSetFmtSpinner->getMinute();
        LOG_INFO("Setting new alarm: %d:%d", newTime->tm_hour, newTime->tm_min);
    }

    void BellAlarmWindowModel::loadData()
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        timeSetWidget->timeSetFmtSpinner->setHour(std::localtime(&now)->tm_hour);
        timeSetWidget->timeSetFmtSpinner->setMinute(std::localtime(&now)->tm_min);
    }

    auto BellAlarmWindowModel::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }
} // namespace app::bell_settings
