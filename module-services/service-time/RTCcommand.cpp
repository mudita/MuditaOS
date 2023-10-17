// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-time/RTCCommand.hpp"
#include <service-time/TimeMessage.hpp>

#include <service-evtmgr/service-evtmgr/EventManagerCommon.hpp>

void RTCCommand::setTime(const struct tm &time)
{
    auto message = std::make_shared<stm::message::UpdateRTCValueFromTmMessage>(time);
    owner->bus.sendUnicast(message, service::name::evt_manager);
}

void RTCCommand::setTime(const time_t &time)
{
    auto message = std::make_shared<stm::message::UpdateRTCValueFromTimestampMessage>(time);
    owner->bus.sendUnicast(message, service::name::evt_manager);
}

void RTCCommand::setTimezone(const std::string &timezone)
{
    auto message = std::make_shared<stm::message::UpdateTimeZoneMessage>(timezone);
    owner->bus.sendUnicast(message, service::name::evt_manager);
}
