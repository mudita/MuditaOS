// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneInterface.hpp"
#include "service-audio/AudioServiceAPI.hpp"
#include "service-cellular/CellularServiceAPI.hpp"

namespace bluetooth
{
    bool CellularInterfaceImpl::answerIncomingCall(sys::Service *service)
    {
        return CellularServiceAPI::AnswerIncomingCall(service);
    }

    bool CellularInterfaceImpl::hangupCall(sys::Service *service)
    {
        return CellularServiceAPI::HangupCall(service);
    }

    bool AudioInterfaceImpl::startAudioRouting(sys::Service *service)
    {
        return AudioServiceAPI::RoutingStart(service);
    }
    bool AudioInterfaceImpl::stopAudioRouting(sys::Service *service)
    {
        return AudioServiceAPI::StopAll(service);
    }
} // namespace bluetooth
