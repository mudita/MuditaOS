// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>
#include <at/ATFactory.hpp>

#include "service-cellular/requests/PinChangeRequest.hpp"
#include <at/ATFactory.hpp>
#include <map>

namespace
{
    //(according to 3GPP TS 22.030 V16.0.0):
    const std::map<std::string, cellular::PinChangeRequest::PassChangeType> serviceCodeToChangeType = {{
        {"04", cellular::PinChangeRequest::PassChangeType::ChangePin},
        {"042", cellular::PinChangeRequest::PassChangeType::ChangePin2},
        {"05", cellular::PinChangeRequest::PassChangeType::ChangePinByPuk},
    }};

    constexpr inline std::string_view changeNetworkPasswordServiceCode = "03";
} // namespace

namespace cellular
{

    PinChangeRequest::PinChangeRequest(const std::string &data, GroupMatch matchGroups)
        : Request(data), requestOldPinOrPuk(matchGroups[magic_enum::enum_integer(PinChangeRegexGroups::OldPassword)]),
          requestNewPin(matchGroups[magic_enum::enum_integer(PinChangeRegexGroups::NewPassword)]),
          requestNewPinRepeat(matchGroups[magic_enum::enum_integer(PinChangeRegexGroups::NewPasswordRepeat)])
    {
        auto &serviceCode = matchGroups[magic_enum::enum_integer(PinChangeRegexGroups::ServiceCode)];
        if (auto it = serviceCodeToChangeType.find(serviceCode); it == serviceCodeToChangeType.end()) {
            throw std::runtime_error(std::string(__FUNCTION__) + ": input service code not supported");
        }
        else {
            passChangeType = it->second;
        }
    }

    std::unique_ptr<PinChangeRequest> PinChangeRequest::create(const std::string &data, GroupMatch matchGroups)
    {
        return std::make_unique<PinChangeRequest>(data, matchGroups);
    }

    auto PinChangeRequest::command() -> at::Cmd
    {
        std::array<std::function<std::string()>, 2> commandParts = {
            [this]() { return this->getOldPinOrPuk(); },
            [this]() { return "," + this->getNewPin(); },
        };

        if (!isValid()) {
            return at::Cmd(std::string());
        }

        at::Cmd cmd("");
        switch (passChangeType) {
        case PassChangeType::ChangePin:
            cmd = at::factory(at::AT::CPWD);
            cmd = cmd + "\"SC\",";
            break;
        case PassChangeType::ChangePin2:
            cmd = at::factory(at::AT::CPWD);
            cmd = cmd + "\"P2\",";
            break;
        case PassChangeType::ChangePinByPuk:
            cmd = at::factory(at::AT::CPIN);
            break;
        };

        for (auto &cmdPart : commandParts) {
            cmd = cmd + cmdPart();
        }

        return cmd;
    }

    auto PinChangeRequest::getOldPinOrPuk() const noexcept -> std::string
    {
        return "\"" + requestOldPinOrPuk + "\"";
    }

    auto PinChangeRequest::getNewPin() const noexcept -> std::string
    {
        return "\"" + requestNewPin + "\"";
    }

    void PinChangeRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    auto PinChangeRequest::isValid() const noexcept -> bool
    {
        return requestNewPin == requestNewPinRepeat && !requestNewPin.empty() && !requestNewPinRepeat.empty() &&
               !requestOldPinOrPuk.empty();
    }
} // namespace cellular
