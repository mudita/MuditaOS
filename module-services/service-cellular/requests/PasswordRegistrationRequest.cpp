// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>
#include <at/ATFactory.hpp>

#include "service-cellular/requests/PasswordRegistrationRequest.hpp"
#include "service-cellular/requests/CallBarringRequest.hpp" // for barringServiceToFacility map

namespace
{
    constexpr inline std::string_view changeNetworkPasswordServiceCode = "03";
}

namespace cellular
{

    std::unique_ptr<PasswordRegistrationRequest> PasswordRegistrationRequest::create(const std::string &data,
                                                                                     GroupMatch matchGroups)
    {
        return std::make_unique<PasswordRegistrationRequest>(data, matchGroups);
    }

    auto PasswordRegistrationRequest::command() -> at::Cmd
    {
        std::vector<commandBuilderFunc> commandParts = {
            [this]() { return getCommandFacility(); },
            [this]() { return getOldPassword(); },
            [this]() { return getNewPassword(); },
        };

        return at::Cmd(buildCommand(at::AT::CPWD, commandParts));
    }

    auto PasswordRegistrationRequest::getCommandFacility() const noexcept -> std::string
    {
        if (requestBarringService.empty()) {
            return "\"" + std::string(CallBarringRequest::allBarringServicesFacilityString) +
                   "\""; //(According to 3GPP TS 22.030 V16.0.0 )
        }
        for (auto &it : CallBarringRequest::barringServiceToFacility) {
            if (it.first == requestBarringService) {
                return "\"" + std::string(it.second) + "\"";
            }
        }
        return std::string();
    }

    auto PasswordRegistrationRequest::getOldPassword() const noexcept -> std::string
    {
        return "\"" + requestOldPassword + "\"";
    }

    auto PasswordRegistrationRequest::getNewPassword() const noexcept -> std::string
    {
        return "\"" + requestNewPassword + "\"";
    }

    void PasswordRegistrationRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    auto PasswordRegistrationRequest::isValid() const noexcept -> bool
    {
        return requestNewPassword == requestNewPasswordRepeat && !requestNewPasswordRepeat.empty() &&
               !requestNewPassword.empty() && !requestOldPassword.empty();
    }
} // namespace cellular
