// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CellularCallRequestHandler.hpp"
#include "service-cellular/CallRequestFactory.hpp"
#include "service-cellular/ServiceCellular.hpp"

#include "Service/Bus.hpp"
#include "Service/Message.hpp"
#include "Service/Timer.hpp"

void CellularCallRequestHandler::handle(IMEIRequest &request, at::Result &result)
{
    if (!request.checkModemResponse(result)) {
        request.setHandled(false);
        return;
    }
    request.setHandled(true);
}

void CellularCallRequestHandler::handle(USSDRequest &request, at::Result &result)
{
    if (!request.checkModemResponse(result)) {
        request.setHandled(false);
        return;
    }
    cellular.ussdState = ussd::State::pullRequestSent;
    cellular.setUSSDTimer();
    request.setHandled(true);
}

void CellularCallRequestHandler::handle(CallRequest &request, at::Result &result)
{
    if (!request.checkModemResponse(result)) {
        request.setHandled(false);
        return;
    }
    // activate call state timer
    cellular.callStateTimer->reload();
    // Propagate "Ringing" notification into system
    sys::Bus::SendMulticast(
        std::make_shared<CellularCallMessage>(CellularCallMessage::Type::Ringing, request.getNumber()),
        sys::BusChannels::ServiceCellularNotifications,
        &cellular);
    request.setHandled(true);
}
