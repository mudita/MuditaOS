// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeUnitsModel.hpp"

#include <service-time/Constants.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <TimeSetSpinner.hpp>
#include <widgets/Text.hpp>

namespace gui
{
    TimeUnitsModel::TimeUnitsModel(app::Application *app) : application(app)
    {}

    TimeUnitsModel::~TimeUnitsModel()
    {
        clearData();
    }

    auto TimeUnitsModel::getCurrentIndex() -> unsigned int
    {
        return getIndex();
    }

    auto TimeUnitsModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto TimeUnitsModel::getItem(gui::Order order) -> gui::SideListItem *
    {
        return getRecord(order);
    }

    void TimeUnitsModel::createData()
    {
        timeSetSpinner = new gui::TimeSetSpinner(style::sidelistview::listItem::x,
                                                 style::sidelistview::listItem::y,
                                                 style::sidelistview::listItem::w,
                                                 style::sidelistview::listItem::h,
                                                 utils::translate("app_bell_settings_time_units_time_message"));
        internalData.push_back(timeSetSpinner);
    }

    void TimeUnitsModel::clearData()
    {
        eraseInternalData();
    }

    void TimeUnitsModel::saveData()
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm *tmp  = localtime(&now);
        tmp->tm_hour    = timeSetSpinner->getHour();
        tmp->tm_min     = timeSetSpinner->getMinute();
        LOG_INFO("Setting new time: %d:%d", tmp->tm_hour, tmp->tm_min);
        sendRtcUpdateTimeMessage(mktime(tmp));
    }

    void TimeUnitsModel::loadData()
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        timeSetSpinner->setHour(std::localtime(&now)->tm_hour);
        timeSetSpinner->setMinute(std::localtime(&now)->tm_min);
    }

    void TimeUnitsModel::sendRtcUpdateTimeMessage(time_t newTime)
    {
        auto msg = std::make_shared<stm::message::TimeChangeRequestMessage>(newTime);
        application->bus.sendUnicast(std::move(msg), service::name::service_time);
    }

} // namespace gui
