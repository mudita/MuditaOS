// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SecurityEndpoint.hpp"

#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>
#include <service-desktop/service-desktop/ServiceDesktop.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <json/json11.hpp>

#include <string>

using namespace parserFSM;

auto SecurityEndpoint::handle(parserFSM::Context &context) -> void
{
    http::Code responseCode;
    switch (context.getMethod()) {
    case http::Method::get:
        responseCode = processStatus(context);
        break;
    case http::Method::put:
        responseCode = processConfiguration(context);
        break;
    case http::Method::post:
        responseCode = processHandshake(context);
        break;
    default:
        responseCode = http::Code::BadRequest;
        break;
    }

    context.setResponseStatus(responseCode);
    parserFSM::MessageHandler::putToSendQueue(context.createSimpleResponse());
}

auto SecurityEndpoint::processHandshake(Context &context) -> http::Code
{
    auto body = context.getBody();
    if (!body[json::usb::id].is_string()) {
        return http::Code::BadRequest;
    }

    auto msg = std::make_shared<sdesktop::usb::USBHandshake>(body[json::usb::id].string_value(),
                                                             body[json::usb::passcode].int_value());
    return toCode(ownerServicePtr->bus.sendUnicast(msg, service::name::service_desktop));
}

auto SecurityEndpoint::processStatus(Context &context) -> http::Code
{
    auto desktopService = dynamic_cast<ServiceDesktop *>(ownerServicePtr);
    auto security       = desktopService->getSecurity()->getEndpointSecurity();
    return security == EndpointSecurity::Allow ? http::Code::OK : http::Code::Forbidden;
}

auto SecurityEndpoint::processConfiguration(Context &context) -> http::Code
{
    auto body = context.getBody();

    std::shared_ptr<sys::DataMessage> msg;

    if (body[json::usb::status].string_value() == json::usb::on) {
        msg = std::make_shared<sdesktop::usb::USBSecurityOn>();
    }
    if (body[json::usb::status].string_value() == json::usb::off) {
        msg = std::make_shared<sdesktop::usb::USBSecurityOff>();
    }

    return toCode(ownerServicePtr->bus.sendUnicast(std::move(msg), service::name::service_desktop));
}
