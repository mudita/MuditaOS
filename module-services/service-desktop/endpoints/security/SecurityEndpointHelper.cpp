// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/security/SecurityEndpointHelper.hpp>
#include <endpoints/message/Sender.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/messages/PreventBlockingRequest.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <apps-common/locks/data/PhoneLockMessages.hpp>

namespace sdesktop::endpoints
{
    using sender::putToSendQueue;

    auto SecurityEndpointHelper::preventBlockingDevice() -> bool
    {
        auto desktopService = dynamic_cast<ServiceDesktop *>(owner);
        auto msg            = std::make_shared<app::manager::PreventBlockingRequest>(desktopService->GetName());
        return desktopService->bus.sendUnicast(std::move(msg), "ApplicationManager");
    }

    auto SecurityEndpointHelper::processPut(Context &context) -> ProcessResult
    {
        auto code = processConfiguration(context);
        return {sent::no, ResponseContext{.status = code}};
    }

    auto SecurityEndpointHelper::processGet(Context &context) -> ProcessResult
    {
        if (context.getBody()[json::messages::category].string_value() == json::usb::phoneLockStatus) {
            return {sent::no, ResponseContext{.status = processStatus(context)}};
        }
        if (context.getBody()[json::messages::category].string_value() == json::usb::phoneLockTime) {
            if (auto phoneLockTime = getPhoneLockTime(context); phoneLockTime > std::time(nullptr)) {
                auto timeLeftToNextAttempt = phoneLockTime - std::time(nullptr);
                context.setResponseBody(json11::Json::object(
                    {{json::usb::phoneLockTime, static_cast<int>(phoneLockTime)},
                     {json::usb::timeLeftToNextAttempt, static_cast<int>(timeLeftToNextAttempt)}}));
                context.setResponseStatus(http::Code::OK);
            }
            else {
                context.setResponseStatus(http::Code::UnprocessableEntity);
            }
            putToSendQueue(context.createSimpleResponse());
            return {sent::yes, std::nullopt};
        }
        return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
    }

    auto SecurityEndpointHelper::processStatus(Context & /*context*/) -> http::Code
    {
        auto desktopService = dynamic_cast<ServiceDesktop *>(owner);
        auto security       = desktopService->getSecurity()->getEndpointSecurity();

        if (security == EndpointSecurity::Allow) {
            preventBlockingDevice();
        }

        return security == EndpointSecurity::Allow ? http::Code::NoContent : http::Code::Forbidden;
    }

    auto SecurityEndpointHelper::getPhoneLockTime(Context & /*context*/) -> time_t
    {
        auto desktopService = static_cast<ServiceDesktop *>(owner);
        return desktopService->getSecurity()->getPhoneLockTime();
    }

    auto SecurityEndpointHelper::passCodeArrayToVecOfInts(const json11::Json::array &passCode)
        -> std::vector<unsigned int>
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
                status   = owner->bus.sendUnicast(std::move(msg), service::name::appmgr)
                               ? http::Code::NoContent
                               : http::Code::InternalServerError;
            }
            catch (const std::exception &e) {
                LOG_ERROR("Passcode decoding exception");
            }
        }

        return status;
    }

} // namespace sdesktop::endpoints
