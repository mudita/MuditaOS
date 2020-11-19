// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>

#include "service-cellular/requests/PasswordRegistrationRequest.hpp"

namespace
{
    // according to EC25&EC21_AT_Commands_Manual_V1.3
    const std::map<std::string, std::string> barringServiceToFacilityMap = {{
        {"33", "AO"},  // BAOC (Bar All Outgoing Calls)
        {"331", "OI"}, // BOIC (Bar Outgoing International Calls)
        {"332", "OX"}, // BOIC-exHC (Bar Outgoing International Calls except to home country)
        {"35", "AI"},  // BAIC (Bar All Incoming Calls)
        {"351", "IR"}, // BIC-Roam (Bar Incoming Calls when Roaming outside the home country)
        {"330", "AB"}, // All barring services
        {"333", "AG"}, // All outgoing barring services
        {"353", "AC"}, // All incoming barring services
        {"", "AB"},    // All incoming barring services (According to 3GPP TS 22.030 V16.0.0 )
    }};
} // namespace

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
        std::array<std::function<std::string()>, 3> commandParts = {
            [this]() { return this->getCommandFacility(); },
            [this]() { return "," + this->getOldPassword(); },
            [this]() { return "," + this->getNewPassword(); },
        };

        if (!isValid()) {
            return std::string();
        }

        std::string cmd(at::factory(at::AT::CPWD));
        for (auto &cmdPart : commandParts) {
            cmd.append(cmdPart());
        }

        return cmd;
    }

    auto PasswordRegistrationRequest::getCommandFacility() const noexcept -> std::string
    {
        if (auto it = barringServiceToFacilityMap.find(requestBarringService);
            it != barringServiceToFacilityMap.end()) {
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
