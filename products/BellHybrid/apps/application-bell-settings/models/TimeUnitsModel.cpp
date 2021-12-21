// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/TimeUnitsModel.hpp"
#include "widgets/TimeFormatSetListItem.hpp"
#include "widgets/TimeSetListItem.hpp"
#include "widgets/TemperatureUnitListItem.hpp"
#include "ProductConfig.hpp"

#include <gui/widgets/ListViewEngine.hpp>
#include <gui/widgets/Style.hpp>
#include <service-time/Constants.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

#include <ctime>

namespace app::bell_settings
{
    TimeUnitsModel::TimeUnitsModel(app::ApplicationCommon *app) : application(app)
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

        timeFmtSetListItem = new gui::TimeFormatSetListItem(
            0, 0, 0, 0, utils::translate("app_bell_settings_time_units_time_fmt_top_message"));
        internalData.push_back(timeFmtSetListItem);

        timeSetListItem =
            new gui::TimeSetListItem(0U, 0U, 0, 0, utils::translate("app_bell_settings_time_units_time_message"));
        internalData.push_back(timeSetListItem);

        timeFmtSetListItem->onNextCallback = [this](gui::Item &) {
            timeSetListItem->timeSetFmtSpinner->setTimeFormat(timeFmtSetListItem->getTimeFmt());
        };

#if CONFIG_ENABLE_TEMP == 1
        temperatureUnitListItem = new gui::TemperatureUnitListItem(utils::translate("app_bell_settings_temp_scale"));
        internalData.push_back(temperatureUnitListItem);
#endif

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
        const auto newTime = timeSetListItem->timeSetFmtSpinner->getTime();
        const auto time_tm = std::localtime(&newTime);
        const auto newFmt  = timeFmtSetListItem->getTimeFmt();
        LOG_INFO("Setting new time: %d:%d fmt: %s",
                 time_tm->tm_hour,
                 time_tm->tm_min,
                 utils::time::Locale::format(newFmt).c_str());
        sendRtcUpdateTimeMessage(newTime);
        sendTimeFmtUpdateMessage(newFmt);
    }

    void TimeUnitsModel::loadData()
    {
        const auto now        = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto timeFormat = stm::api::timeFormat();
        timeSetListItem->timeSetFmtSpinner->setTimeFormat(timeFormat);
        timeSetListItem->timeSetFmtSpinner->setTime(now);
        timeFmtSetListItem->setTimeFmt(timeFormat);
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

    void TimeUnitsModel::sendTimeFmtUpdateMessage(utils::time::Locale::TimeFormat newFmt)
    {
        auto msg = std::make_shared<stm::message::SetTimeFormatRequest>(newFmt);
        application->bus.sendUnicast(std::move(msg), service::name::service_time);
    }

    auto TimeUnitsModel::getTemperatureUnit() const -> utils::temperature::Temperature::Unit
    {
#if CONFIG_ENABLE_TEMP == 1
        return *utils::temperature::strToUnit(temperatureUnitListItem->getUnitAsStr());
#else
        return utils::temperature::Temperature::Unit::Celsius;
#endif
    }

    auto TimeUnitsModel::setTemperatureUnit(const utils::temperature::Temperature::Unit unit) -> void
    {
#if CONFIG_ENABLE_TEMP == 1
        temperatureUnitListItem->setUnit(unit);
#endif
    }

    void TimeUnitsModelFactoryResetValues::loadData()
    {
        timeSetListItem->timeSetFmtSpinner->setTimeFormat(factoryRestTimeFmt);
        timeSetListItem->timeSetFmtSpinner->setTime(factoryResetTime);
        timeFmtSetListItem->setTimeFmt(factoryRestTimeFmt);
    }
} // namespace app::bell_settings
