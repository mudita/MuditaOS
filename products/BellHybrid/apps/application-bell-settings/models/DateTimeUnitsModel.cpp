// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/DateTimeUnitsModel.hpp"
#include "widgets/DateSetListItem.hpp"
#include "widgets/TimeFormatSetListItem.hpp"
#include "widgets/TimeSetListItem.hpp"
#include "widgets/TemperatureUnitListItem.hpp"
#include "ProductConfig.hpp"

#include <gui/widgets/ListViewEngine.hpp>
#include <gui/widgets/Style.hpp>
#include <service-time/Constants.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <widgets/DateSetSpinner.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

#include <ctime>

namespace app::bell_settings
{
    DateTimeUnitsModel::DateTimeUnitsModel(app::ApplicationCommon *app) : application(app)
    {}

    DateTimeUnitsModel::~DateTimeUnitsModel()
    {
        clearData();
    }

    auto DateTimeUnitsModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto DateTimeUnitsModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    auto DateTimeUnitsModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    void DateTimeUnitsModel::createData()
    {
        dateSetListItem = new gui::DateSetListItem();
        internalData.push_back(dateSetListItem);

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

    void DateTimeUnitsModel::clearData()
    {
        eraseInternalData();
    }

    void DateTimeUnitsModel::saveData()
    {
        const auto date   = dateSetListItem->dateSetSpinner->getDate();
        const auto year   = date.year().operator int();
        const auto month  = static_cast<int>(date.month().operator unsigned int());
        const auto day    = static_cast<int>(date.day().operator unsigned int());
        const auto hour   = timeSetListItem->timeSetFmtSpinner->getHour24Format();
        const auto minute = timeSetListItem->timeSetFmtSpinner->getMinute();
        const auto fmt    = timeFmtSetListItem->getTimeFmt();

        const auto newTime = GetAsUTCTime(year, month, day, hour.count(), minute);

        LOG_DEBUG("Setting a new date/time: %d/%d/%d %d:%d %s",
                  year,
                  month,
                  day,
                  static_cast<int>(hour.count()),
                  minute,
                  utils::time::Locale::get_time_format(fmt).data());

        sendRtcUpdateTimeMessage(newTime);
        sendTimeFmtUpdateMessage(fmt);
    }

    void DateTimeUnitsModel::loadData()
    {
        const auto now        = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto timeFormat = stm::api::timeFormat();
        timeSetListItem->timeSetFmtSpinner->setTimeFormat(timeFormat);
        timeSetListItem->timeSetFmtSpinner->setTime(now);
        dateSetListItem->dateSetSpinner->setDate(
            date::year_month_day{date::floor<date::days>(std::chrono::system_clock::now())});

        timeFmtSetListItem->setTimeFmt(timeFormat);
    }

    auto DateTimeUnitsModel::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    void DateTimeUnitsModel::sendRtcUpdateTimeMessage(time_t newTime)
    {
        auto msg = std::make_shared<stm::message::TimeChangeRequestMessage>(newTime);
        application->bus.sendUnicast(std::move(msg), service::name::service_time);
    }

    void DateTimeUnitsModel::sendTimeFmtUpdateMessage(utils::time::Locale::TimeFormat newFmt)
    {
        auto msg = std::make_shared<stm::message::SetTimeFormatRequest>(newFmt);
        application->bus.sendUnicast(std::move(msg), service::name::service_time);
    }

    auto DateTimeUnitsModel::getTemperatureUnit() const -> utils::temperature::Temperature::Unit
    {
#if CONFIG_ENABLE_TEMP == 1
        return *utils::temperature::strToUnit(temperatureUnitListItem->getUnitAsStr());
#else
        return utils::temperature::Temperature::Unit::Celsius;
#endif
    }

    auto DateTimeUnitsModel::getTimeFormat() const -> utils::time::Locale::TimeFormat
    {
        return timeFmtSetListItem->getTimeFmt();
    }

    auto DateTimeUnitsModel::setTemperatureUnit(const utils::temperature::Temperature::Unit unit) -> void
    {
#if CONFIG_ENABLE_TEMP == 1
        temperatureUnitListItem->setUnit(unit);
#endif
    }

    void DateTimeUnitsModelFactoryResetValues::loadData()
    {
        using namespace date::literals;

        /// Default date/time after factory reset: 2022/01/01 12:00PM
        const auto factoryResetDate   = 2022_y / jan / 1_d;
        const auto factoryRestTimeFmt = utils::time::Locale::TimeFormat::FormatTime12H;

        dateSetListItem->dateSetSpinner->setDate(factoryResetDate);
        timeSetListItem->timeSetFmtSpinner->setTimeFormat(factoryRestTimeFmt);
        timeSetListItem->timeSetFmtSpinner->setHour(12);
        timeSetListItem->timeSetFmtSpinner->setMinute(0);
        timeSetListItem->timeSetFmtSpinner->set12HPeriod(gui::TimeSetFmtSpinner::Period::PM);
        timeFmtSetListItem->setTimeFmt(factoryRestTimeFmt);
    }
} // namespace app::bell_settings
