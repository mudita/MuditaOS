// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>
#include <at/ATFactory.hpp>

#include "service-cellular/requests/CallBarringRequest.hpp"

namespace cellular
{

    std::unique_ptr<SupplementaryServicesRequest> CallBarringRequest::create(const std::string &serviceCode,
                                                                             const std::string &data,
                                                                             GroupMatch matchGroups)
    {
        for (auto &it : barringServiceToFacility) {
            if (it.first == serviceCode) {
                return std::make_unique<CallBarringRequest>(std::string(it.second), data, matchGroups);
            }
        }

        return nullptr;
    }

    auto CallBarringRequest::command() -> at::Cmd
    {
        std::vector<commandBuilderFunc> commandParts = {
            [this]() { return getCommandFacility(); },
            [this]() { return getCommandMode(); },
            [this]() { return getCommandPassword(); },
            [this]() { return getCommandClass(); },
        };

        return buildCommand(at::AT::CLCK, commandParts);
    }

    auto CallBarringRequest::getCommandFacility() const -> std::string
    {
        return "\"" + facility + "\"";
    }

    auto CallBarringRequest::getCommandMode() const -> std::string
    {
        return utils::to_string(magic_enum::enum_integer(procedureType));
    }

    auto CallBarringRequest::getCommandPassword() const -> std::string
    {
        return password.empty() ? std::string() : "\"" + password + "\"";
    }

    auto CallBarringRequest::getCommandClass() const -> std::string
    {
        if (basicServiceGroup.empty()) {
            return std::string();
        }

        return getCommandInformationClass(basicServiceGroup).value_or(std::string());
    }

    void CallBarringRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    auto CallBarringRequest::isValid() const noexcept -> bool
    {
        if (!getCommandInformationClass(basicServiceGroup).has_value()) {
            return false;
        }
        return procedureType != ProcedureType::Registration && procedureType != ProcedureType::Erasure;
    }

} // namespace cellular
