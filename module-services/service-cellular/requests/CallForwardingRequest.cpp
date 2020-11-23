// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>

#include "service-cellular/requests/CallForwardingRequest.hpp"

namespace
{
    // according to EC25&EC21_AT_Commands_Manual_V1.3
    const std::map<std::string, std::string> reasonCodes = {{
        {"21", "0"},  // Unconditional
        {"67", "1"},  // MobileBusy
        {"61", "2"},  // NoReply
        {"62", "3"},  // NotReachable
        {"002", "4"}, // AllCallForwarding
        {"004", "5"}, // AllConditionalCallForwarding
    }};
} // namespace

namespace cellular
{

    std::unique_ptr<SupplementaryServicesRequest> CallForwardingRequest::create(const std::string &serviceCode,
                                                                                const std::string &data,
                                                                                GroupMatch matchGroups)
    {
        if (auto it = reasonCodes.find(serviceCode); it != reasonCodes.end()) {
            return std::make_unique<CallForwardingRequest>(it->second, data, matchGroups);
        }

        return nullptr;
    }

    auto CallForwardingRequest::command() -> std::string
    {
        std::vector<commandBuilderFunc> commandParts = {[this]() { return getCommandReason(); },
                                                        [this]() { return getCommandMode(); },
                                                        [this]() { return getCommandNumber(); },
                                                        [this]() { return getCommandType(); },
                                                        [this]() { return getCommandClass(); },
                                                        [this]() { return getCommandSubAddr(); },
                                                        [this]() { return getCommandSatype(); },
                                                        [this]() { return getCommandTime(); }};

        const auto itToCommandClass  = std::next(commandParts.begin(), 5);
        const auto itToCommandNumber = std::next(commandParts.begin(), 3);

        std::vector<commandBuilderFunc> trimmedCommandParts;

        if (!getCommandTime().empty()) {
            return buildCommand(at::AT::CCFC, commandParts, false);
        }
        else if (!getCommandClass().empty()) {
            trimmedCommandParts.insert(trimmedCommandParts.begin(), commandParts.begin(), itToCommandClass);
        }
        else {
            trimmedCommandParts.insert(trimmedCommandParts.begin(), commandParts.begin(), itToCommandNumber);
        }

        return buildCommand(at::AT::CCFC, trimmedCommandParts);
    }

    auto CallForwardingRequest::getCommandReason() const -> std::string
    {
        return forwardReason;
    }

    auto CallForwardingRequest::getCommandMode() const -> std::string
    {
        return utils::to_string(magic_enum::enum_integer(procedureType));
    }

    auto CallForwardingRequest::getCommandNumber() const -> std::string
    {
        return phoneNumber.empty() ? std::string() : "\"" + phoneNumber + "\"";
    }

    auto CallForwardingRequest::getCommandType() const -> std::string
    {
        // according to EC25&EC21_AT_Commands_Manual_V1.3
        if (auto pos = phoneNumber.find("+"); pos != std::string::npos) {
            return addressFormatTypeInternational;
        }
        else {
            return addressFormatTypeDefault;
        }
    }

    auto CallForwardingRequest::getCommandClass() const -> std::string
    {
        if (basicServiceGroup.empty()) {
            return std::string();
        }

        return getCommandInformationClass(basicServiceGroup).value_or(std::string());
    }

    auto CallForwardingRequest::getCommandSubAddr() const -> std::string
    {
        return subaddrDefault;
    }

    auto CallForwardingRequest::getCommandSatype() const -> std::string
    {
        return std::string();
    }

    auto CallForwardingRequest::getCommandTime() const -> std::string
    {
        return noReplyConditionTimer;
    }

    auto CallForwardingRequest::isValid() const noexcept -> bool
    {
        if (noReplyConditionTimer.empty()) {
            return true;
        }
        int time;
        if (utils::toNumeric(noReplyConditionTimer, time)) {
            // EC25&EC21_AT_Commands_Manual
            return std::clamp(time, 0, 30) == time;
        }
        return false;
    }

    void CallForwardingRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
} // namespace cellular
