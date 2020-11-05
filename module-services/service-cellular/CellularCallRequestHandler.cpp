// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CellularCallRequestHandler.hpp"
#include "CallRequestFactory.hpp"
#include "ServiceCellular.hpp"

#include "Service/Bus.hpp"
#include "Service/Message.hpp"
#include "Service/Timer.hpp"

void CellularCallRequestHandler::handle(IMEIRequest &request)
{}
void CellularCallRequestHandler::handle(USSDRequest &request)
{
    cellular.ussdState = ussd::State::pullRequestSent;
    cellular.setUSSDTimer();
}
void CellularCallRequestHandler::handle(CallRequest &request)
{

    // activate call state timer
    cellular.callStateTimer->reload();
    // Propagate "Ringing" notification into system
    sys::Bus::SendMulticast(
        std::make_shared<CellularCallMessage>(CellularCallMessage::Type::Ringing, request.getNumber()),
        sys::BusChannels::ServiceCellularNotifications,
        &cellular);
}
