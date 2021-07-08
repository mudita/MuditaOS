// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceAlarmMessages.hpp"

#include <Service/Service.hpp>

#include <memory>

namespace ServiceAlarmAPI
{
    /**
     * @brief Sends alarm request, response message AlarmResponseMessage
     * @param serv Requesting service
     * @param alarm Requested alarm
     * @return True if request has been sent successfully, false otherwise
     */
    bool SetAlarm(sys::Service *serv, std::shared_ptr<AlarmObject> alarm);
} // namespace ServiceAlarmAPI
