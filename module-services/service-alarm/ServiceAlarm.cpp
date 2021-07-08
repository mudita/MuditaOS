// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceAlarm.hpp"
#include "ServiceAlarmImp.hpp"
#include "ServiceAlarmMessages.hpp"

#include <log.hpp>

inline constexpr auto ServiceAlarmStackSize = 4096;

ServiceAlarm::ServiceAlarm() : sys::Service(service::name::alarm, "", ServiceAlarmStackSize, sys::ServicePriority::Idle)
{
    LOG_INFO("Initializing");

    connect(typeid(AlarmSetMessage),
            [this](sys::Message *message) -> sys::MessagePointer { return impl.SetAlarm(message); });
}

ServiceAlarm::~ServiceAlarm()
{
    LOG_INFO("Cleaning resources");
}

sys::ReturnCodes ServiceAlarm::InitHandler()
{
    impl.Init();

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceAlarm::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

void ServiceAlarm::ProcessCloseReason(sys::CloseReason closeReason)
{
    sendCloseReadyMessage(this);
}

sys::ReturnCodes ServiceAlarm::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    return sys::ReturnCodes::Success;
}

sys::MessagePointer ServiceAlarm::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    return std::make_shared<sys::ResponseMessage>();
}
