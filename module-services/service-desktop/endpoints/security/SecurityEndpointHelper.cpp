// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SecurityEndpointHelper.hpp"
#include "service-db/SettingsMessages.hpp"
#include <service-desktop/DesktopMessages.hpp>
#include <parser/ParserUtils.hpp>
#include <Service/Common.hpp>
#include <Service/Service.hpp>
#include <service-desktop/parser/MessageHandler.hpp>
#include <service-desktop/service-desktop/ServiceDesktop.hpp>
#include <json/json11.hpp>

namespace parserFSM
{
    class Context;
} // namespace parserFSM

using namespace parserFSM;

auto SecurityEndpointHelper::processPut(Context &context) -> ProcessResult
{
    auto code = processConfiguration(context);
    return {sent::no, endpoint::ResponseContext{.status = code}};
}

auto SecurityEndpointHelper::processGet(Context &context) -> ProcessResult
{
    auto code = processStatus(context);
    return {sent::no, endpoint::ResponseContext{.status = code}};
}

auto SecurityEndpointHelper::processPost(Context &context) -> ProcessResult
{
    auto code = processHandshake(context);
    return {sent::no, endpoint::ResponseContext{.status = code}};
}

auto SecurityEndpointHelper::processHandshake(Context &context) -> http::Code
{
    auto body = context.getBody();
    if (!body[json::usb::id].is_string()) {
        return http::Code::BadRequest;
    }

    auto msg = std::make_shared<sdesktop::usb::USBHandshake>(body[json::usb::id].string_value(),
                                                             body[json::usb::passcode].int_value());
    return toCode(owner->bus.sendUnicast(msg, service::name::service_desktop));
}

auto SecurityEndpointHelper::processStatus(Context &context) -> http::Code
{
    auto desktopService = dynamic_cast<ServiceDesktop *>(owner);
    auto security       = desktopService->getSecurity()->getEndpointSecurity();

    return security == EndpointSecurity::Allow ? http::Code::OK : http::Code::Forbidden;
}

auto SecurityEndpointHelper::processConfiguration(Context &context) -> http::Code
{
    auto body = context.getBody();
    auto code = http::Code::BadRequest;

    std::shared_ptr<sys::DataMessage> msg;

    if (body[json::usb::status].string_value() == json::usb::on) {
        msg = std::make_shared<sdesktop::usb::USBSecurityOn>();
    }
    if (body[json::usb::status].string_value() == json::usb::off) {
        msg = std::make_shared<sdesktop::usb::USBSecurityOff>();
    }

    if (msg) {
        code = toCode(owner->bus.sendUnicast(std::move(msg), service::name::service_desktop));
    }

    return code;
}
