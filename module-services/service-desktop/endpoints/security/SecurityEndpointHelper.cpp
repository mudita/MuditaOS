// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SecurityEndpointHelper.hpp"
#include <parser/ParserUtils.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/messages/PreventBlockingRequest.hpp>
#include <service-desktop/service-desktop/ServiceDesktop.hpp>
#include <module-apps/locks/data/PhoneLockMessages.hpp>
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

    return security == EndpointSecurity::Allow ? http::Code::OK : http::Code::Forbidden;
}

auto SecurityEndpointHelper::passCodeStringToVecOfInts(const std::string &passCode) -> std::vector<unsigned int>
{
    std::vector<unsigned int> passCodeAsInts(0, 0);

    for (auto i = 0u; i < passCode.length(); i++) {
        auto c         = passCode.at(i);
        unsigned int v = std::atoi(&c);
        passCodeAsInts.push_back(v);
    }

    return passCodeAsInts;
}

auto SecurityEndpointHelper::processConfiguration(Context &context) -> http::Code
{
    auto body     = context.getBody();
    auto passCode = body[json::usb::phoneLockCode].string_value();

    if (passCode.length() != PasscodeLength) {
        return http::Code::BadRequest;
    }
    auto passCodeAsInts = passCodeStringToVecOfInts(passCode);

    auto msg = std::make_shared<locks::ExternalUnLockPhone>(passCodeAsInts);

    return toCode(owner->bus.sendUnicast(std::move(msg), app::manager::ApplicationManager::ServiceName));
}
