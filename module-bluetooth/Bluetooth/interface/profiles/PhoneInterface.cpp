// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneInterface.hpp"
#include "service-audio/AudioServiceAPI.hpp"
#include "service-cellular/CellularServiceAPI.hpp"

namespace bluetooth
{
    bool CellularInterfaceImpl::answerIncomingCall(sys::Service *service)
    {
        callActive = true;
        return CellularServiceAPI::AnswerIncomingCall(service);
    }

    bool CellularInterfaceImpl::hangupCall(sys::Service *service)
    {
        if (callActive) {
            callActive = false;
            return CellularServiceAPI::HangupCall(service);
        }
        return true;
    }

    bool AudioInterfaceImpl::startAudioRouting(sys::Service *service)
    {
        return AudioServiceAPI::RoutingStart(service);
    }
} // namespace bluetooth
