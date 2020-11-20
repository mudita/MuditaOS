// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>

#include "service-cellular/requests/CallWaitingRequest.hpp"

namespace cellular
{

    std::unique_ptr<SupplementaryServicesRequest> CallWaitingRequest::create(const std::string &serviceCode,
                                                                             const std::string &data,
                                                                             GroupMatch matchGroups)
    {
        if (serviceCode == callWaitingServiceCode) {
            return std::make_unique<CallWaitingRequest>(data, matchGroups);
        }
        else {
            return nullptr;
        }
    }

    auto CallWaitingRequest::command() -> std::string
    {
        if (!isValid()) {
            return std::string();
        }

        std::array<std::function<std::string()>, 3> commandParts = {
            [this]() { return this->getCommandPresentation(); },
            [this]() { return this->getCommandMode(); },
            [this]() { return this->getCommandClass(); },
        };

        std::string cmd(at::factory(at::AT::CCWA));
        bool formatFirst = true;
        for (auto &cmdPart : commandParts) {
            auto partStr = cmdPart();
            if (partStr.empty()) {
                continue;
            }
            cmd.append(formatFirst ? partStr : "," + partStr);
            formatFirst = false;
        }

        return cmd;
    }

    auto CallWaitingRequest::getCommandPresentation() const noexcept -> std::string
    {
        // fixed, we always want full report
        return unsolicitedResultCodeEnable;
    }

    auto CallWaitingRequest::getCommandMode() const noexcept -> std::string
    {
        return utils::to_string(magic_enum::enum_integer(procedureType));
    }

    auto CallWaitingRequest::getCommandClass() const noexcept -> std::string
    {
        if (basicServiceGroup.empty()) {
            return std::string();
        }

        return getCommandInformationClass(basicServiceGroup).value_or(std::string());
    }

    void CallWaitingRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    auto CallWaitingRequest::isValid() const noexcept -> bool
    {
        if (!getCommandInformationClass(basicServiceGroup).has_value()) {
            return false;
        }
        return procedureType != ProcedureType::Registration && procedureType != ProcedureType::Erasure;
    }

} // namespace cellular
