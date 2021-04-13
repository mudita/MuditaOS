// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>
#include <at/ATFactory.hpp>

#include "service-cellular/requests/CallForwardingRequest.hpp"
#include <map>

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

    auto CallForwardingRequest::command() -> at::Cmd
    {
        std::vector<commandBuilderFunc> commandParts = {[this]() { return getCommandReason(); },
                                                        [this]() { return getCommandMode(); },
                                                        [this]() { return getCommandNumber(); }};

        bool trimEmpty = true;

        if (!getCommandClass().empty()) {
            commandParts.emplace_back([this]() { return getCommandType(); });
            commandParts.emplace_back([this]() { return getCommandClass(); });
            trimEmpty = false;
        }

        if (!getCommandTime().empty()) {
            commandParts.emplace_back([this]() { return getCommandSubAddr(); });
            commandParts.emplace_back([this]() { return getCommandSatype(); });
            commandParts.emplace_back([this]() { return getCommandTime(); });
            trimEmpty = false;
        }

        return buildCommand(at::AT::CCFC, commandParts, trimEmpty);
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
        auto time = utils::getNumericValue<int>(noReplyConditionTimer);
        return std::clamp(time, minNoReplyTime, maxNoReplyTime) == time;
    }

    void CallForwardingRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
} // namespace cellular
