// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/DateTimeUnitsModel.hpp"
#include "widgets/DateSetListItem.hpp"
#include "widgets/TimeFormatSetListItem.hpp"
#include "widgets/DateFormatSetListItem.hpp"
#include "widgets/TimeSetListItem.hpp"
#include "widgets/TemperatureUnitListItem.hpp"
#include "ProductConfig.hpp"

#include <gui/widgets/ListViewEngine.hpp>
#include <gui/widgets/Style.hpp>
#include <service-time/ServiceTimeName.hpp>
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
        yearSetListItem = new gui::YearSetListItem(utils::translate("app_settings_title_year"));
        internalData.push_back(yearSetListItem);

        monthSetListItem = new gui::MonthSetListItem(utils::translate("app_settings_title_month"));
        internalData.push_back(monthSetListItem);

        daySetListItem = new gui::DaySetListItem(utils::translate("app_settings_title_day"));
        internalData.push_back(daySetListItem);

        dateFmtSetListItem = new gui::DateFormatSetListItem(
            0, 0, 0, 0, utils::translate("app_bell_settings_time_units_date_fmt_top_message"));
        internalData.push_back(dateFmtSetListItem);

        timeSetListItem =
            new gui::TimeSetListItem(0U, 0U, 0, 0, utils::translate("app_bell_settings_time_units_time_message"));
        internalData.push_back(timeSetListItem);
        timeSetListItem->getTimeFormat = [this]() { return timeFmtSetListItem->getTimeFmt(); };

        timeFmtSetListItem = new gui::TimeFormatSetListItem(
            0, 0, 0, 0, utils::translate("app_bell_settings_time_units_time_fmt_top_message"));
        internalData.push_back(timeFmtSetListItem);

#if CONFIG_ENABLE_TEMP == 1
        temperatureUnitListItem = new gui::TemperatureUnitListItem(utils::translate("app_bell_settings_temp_scale"));
        internalData.push_back(temperatureUnitListItem);
#endif

        yearSetListItem->onNextCallback = [this](gui::Item &) {
            const auto date = yearSetListItem->dateSetSpinner->getDate();
            monthSetListItem->dateSetSpinner->setDate(date);
            daySetListItem->dateSetSpinner->setDate(date);
        };

        monthSetListItem->onNextCallback = [this](gui::Item &) {
            const auto date = monthSetListItem->dateSetSpinner->getDate();
            daySetListItem->dateSetSpinner->setDate(date);
            yearSetListItem->dateSetSpinner->setDate(date);
        };

        daySetListItem->onNextCallback = [this](gui::Item &) {
            const auto date = daySetListItem->dateSetSpinner->getDate();
            yearSetListItem->dateSetSpinner->setDate(date);
            monthSetListItem->dateSetSpinner->setDate(date);
        };

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
        const auto date       = daySetListItem->dateSetSpinner->getDate();
        const auto year       = date.year().operator int();
        const auto month      = static_cast<int>(date.month().operator unsigned int());
        const auto day        = static_cast<int>(date.day().operator unsigned int());
        const auto hour       = timeSetListItem->timeSetSpinner->getHour24Format();
        const auto minute     = timeSetListItem->timeSetSpinner->getMinute();
        const auto timeFormat = timeFmtSetListItem->getTimeFmt();
        const auto dateFormat = dateFmtSetListItem->getDateFmt();

        const auto newTime = GetAsUTCTime(year, month, day, hour, minute);

        LOG_DEBUG("Setting a new date/time: %02d/%02d/%d (%s) %d:%02d (%s)",
                  (dateFormat == utils::time::Locale::DateFormat::DD_MM_YYYY) ? day : month,
                  (dateFormat == utils::time::Locale::DateFormat::DD_MM_YYYY) ? month : day,
                  year,
                  utils::time::Locale::get_date_format(dateFormat).data(),
                  hour,
                  minute,
                  utils::time::Locale::get_time_format(timeFormat).data());

        sendRtcUpdateTimeMessage(newTime);
        sendTimeFmtUpdateMessage(timeFormat);
        sendDateFmtUpdateMessage(dateFormat);
    }

    void DateTimeUnitsModel::loadData()
    {
        const auto now        = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto timeFormat = stm::api::timeFormat();
        const auto dateFormat = stm::api::dateFormat();
        timeSetListItem->timeSetSpinner->setTimeFormat(timeFormat);
        timeSetListItem->timeSetSpinner->setTime(now);
        yearSetListItem->dateSetSpinner->setDate(
            date::year_month_day{date::floor<date::days>(std::chrono::system_clock::now())});

        timeFmtSetListItem->setTimeFmt(timeFormat);
        dateFmtSetListItem->setDateFmt(dateFormat);
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

    void DateTimeUnitsModel::sendDateFmtUpdateMessage(utils::time::Locale::DateFormat newFmt)
    {
        auto msg = std::make_shared<stm::message::SetDateFormatRequest>(newFmt);
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

        /// Default date/time after factory reset: 2023/01/01 12:00 (in 24h format)
        const auto factoryResetDate    = 2023_y / jan / 1_d;
        const auto factoryResetTimeFmt = utils::time::Locale::TimeFormat::FormatTime24H;
        const auto factoryResetDateFmt = utils::time::Locale::DateFormat::DD_MM_YYYY;

        yearSetListItem->dateSetSpinner->setDate(factoryResetDate);
        timeSetListItem->timeSetSpinner->setTimeFormat(factoryResetTimeFmt);
        timeSetListItem->timeSetSpinner->setHour(12);
        timeSetListItem->timeSetSpinner->setMinute(0);
        timeFmtSetListItem->setTimeFmt(factoryResetTimeFmt);
        dateFmtSetListItem->setDateFmt(factoryResetDateFmt);
    }
} // namespace app::bell_settings
