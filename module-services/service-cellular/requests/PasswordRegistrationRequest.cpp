// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>

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

    auto PasswordRegistrationRequest::command() -> std::string
    {
        std::vector<std::function<std::string()>> commandParts = {
            [this]() { return this->getCommandFacility(); },
            [this]() { return this->getOldPassword(); },
            [this]() { return this->getNewPassword(); },
        };

        return buildCommand(at::AT::CPWD, commandParts);
    }

    auto PasswordRegistrationRequest::getCommandFacility() const noexcept -> std::string
    {
        if (auto it = barringServiceToFacility.find(requestBarringService); it != barringServiceToFacility.end()) {
            return "\"" + it->second + "\"";
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
