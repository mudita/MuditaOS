// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>

#include "service-cellular/MmiRequestCallForwarding.hpp"

namespace call_request
{
    //
    std::unique_ptr<MmiRequestCallForwarding> MmiRequestCallForwarding::create(const std::string &serviceCode,
                                                                               const std::string &data,
                                                                               GroupMatch matchGroups)
    {
        // according to EC25&EC21_AT_Commands_Manual_V1.3
        std::array<std::pair<std::string, std::string>, 6> reasonCodes = {{
            {"21", "0"},  // Unconditional
            {"67", "1"},  // MobileBusy
            {"61", "2"},  // NoReply
            {"62", "3"},  // NotReachable
            {"002", "4"}, // AllCallForwarding
            {"004", "5"}, // AllConditionalCallForwarding
        }};

        for (auto &code : reasonCodes) {
            if (serviceCode == code.first) {
                return std::make_unique<MmiRequestCallForwarding>(code.second, data, matchGroups);
            }
        }

        return nullptr;
    }

    auto MmiRequestCallForwarding::command() -> std::string
    {
        std::array<std::function<std::string()>, 2> commandParts = {
            [this]() { return this->getCommandMode(); },
            [this]() { return this->getCommandNumber(); },
        };

        std::string cmd(at::factory(at::AT::CCFC) + this->getCommandReason());
        for (auto &cmdPart : commandParts) {
            auto partStr = cmdPart();
            if (partStr.empty()) {
                break;
            }
            cmd.append("," + partStr);
        }

        return cmd;
    }

    auto MmiRequestCallForwarding::getCommandReason() -> std::string
    {
        return forwardReason;
    }

    auto MmiRequestCallForwarding::getCommandMode() -> std::string
    {
        return utils::to_string(magic_enum::enum_integer(procedureType));
    }

    auto MmiRequestCallForwarding::getCommandNumber() -> std::string
    {
        return phoneNumber.empty() ? std::string() : "\"" + phoneNumber + "\"";
    }

    auto MmiRequestCallForwarding::getCommandType() -> std::string
    {
        // according to EC25&EC21_AT_Commands_Manual_V1.3
        if (auto pos = phoneNumber.find("+"); pos != std::string::npos) {
            return "129";
        }
        else {
            return "145";
        }
    }

    auto MmiRequestCallForwarding::getCommandClass() -> std::string
    {
        // according to EC25&EC21_AT_Commands_Manual_V1.3
        if (basicSericeGroup == "11") {
            // voice
            return "1";
        }
        else if (basicSericeGroup == "13") {
            // fax
            return "4";
        }
        else if (basicSericeGroup == "25") {
            // data
            return "2";
        }
        // all
        return "7";
    }

    auto MmiRequestCallForwarding::getCommandSubAddr() -> std::string
    {
        // default value not used
        return "128";
    }

    auto MmiRequestCallForwarding::getCommandSatype() -> std::string
    {
        // default value not used
        return "";
    }

    auto MmiRequestCallForwarding::getCommandTime() -> std::string
    {
        return noReplyConditionTimer;
    }

    void MmiRequestCallForwarding::handle(CallRequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
} // namespace call_request
