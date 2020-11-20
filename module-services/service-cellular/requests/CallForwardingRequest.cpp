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
        std::vector<std::function<std::string()>> commandParts = {
            [this]() { return this->getCommandReason(); },
            [this]() { return this->getCommandMode(); },
            [this]() { return this->getCommandNumber(); },
        };

        return buildCommand(at::AT::CCFC, commandParts);
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
            return addressFormatTypeDefault;
        }
        else {
            return addressFormatTypeInternational;
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

    void CallForwardingRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
} // namespace cellular
