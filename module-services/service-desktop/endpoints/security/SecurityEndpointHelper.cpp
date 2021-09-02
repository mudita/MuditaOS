// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SecurityEndpointHelper.hpp"
#include <parser/ParserUtils.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/messages/PreventBlockingRequest.hpp>
#include <service-desktop/service-desktop/ServiceDesktop.hpp>
#include <apps-common/locks/data/PhoneLockMessages.hpp>
#include <json11.hpp>

namespace parserFSM
{
    class Context;
} // namespace parserFSM

using namespace parserFSM;

auto SecurityEndpointHelper::preventBlockingDevice() -> bool
{
    auto desktopService = dynamic_cast<ServiceDesktop *>(owner);
    auto msg            = std::make_shared<app::manager::PreventBlockingRequest>(desktopService->GetName());
    return desktopService->bus.sendUnicast(std::move(msg), "ApplicationManager");
}

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

auto SecurityEndpointHelper::processStatus(Context &context) -> http::Code
{
    auto desktopService = dynamic_cast<ServiceDesktop *>(owner);
    auto security       = desktopService->getSecurity()->getEndpointSecurity();

    if (security == EndpointSecurity::Allow) {
        preventBlockingDevice();
    }

    return security == EndpointSecurity::Allow ? http::Code::NoContent : http::Code::Forbidden;
}

auto SecurityEndpointHelper::passCodeArrayToVecOfInts(const json11::Json::array &passCode) -> std::vector<unsigned int>
{
    std::vector<unsigned int> passCodeAsInts(0, 0);

    for (const auto &value : passCode) {
        if (value.is_number()) {
            auto v = value.number_value();
            passCodeAsInts.push_back(v);
        }
        else {
            throw std::invalid_argument("value not a digit");
        }
    }

    return passCodeAsInts;
}

auto SecurityEndpointHelper::processConfiguration(Context &context) -> http::Code
{
    auto body     = context.getBody();
    auto passCode = body[json::usb::phoneLockCode].array_items();
    http::Code status{http::Code::BadRequest};

    if (passCode.size() == PasscodeLength) {
        try {
            auto msg = std::make_shared<locks::ExternalUnLockPhone>(passCodeArrayToVecOfInts(passCode));
            status   = owner->bus.sendUnicast(std::move(msg), service::name::appmgr) ? http::Code::NoContent
                                                                                     : http::Code::InternalServerError;
        }
        catch (const std::exception &e) {
            LOG_ERROR("Passcode decoding exception");
        }
    }

    return status;
}
