// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceAlarm.hpp"
#include "ServiceAlarmAPI.hpp"
#include "ServiceAlarmMessages.hpp"

namespace ServiceAlarmAPI
{
    bool SetAlarm(sys::Service *serv, std::shared_ptr<AlarmObject> alarm)
    {
        auto msg = std::make_shared<AlarmSetMessage>(std::move(alarm));
        return serv->bus.sendUnicast(msg, service::name::alarm);
    }
} // namespace ServiceAlarmAPI
