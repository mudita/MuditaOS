// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>
#include <at/ATFactory.hpp>

#include "service-cellular/requests/ClirRequest.hpp"

namespace cellular
{

    std::unique_ptr<SupplementaryServicesRequest> ClirRequest::create(const std::string &serviceCode,
                                                                      const std::string &data,
                                                                      GroupMatch matchGroups)
    {
        if (serviceCode == clirServiceCode) {
            return std::make_unique<ClirRequest>(data, matchGroups);
        }
        else {
            return nullptr;
        }
    }

    auto ClirRequest::command() -> at::Cmd
    {
        switch (procedureType) {
        case ProcedureType::Deactivation:
            return at::factory(at::AT::CLIR_DISABLE);
        case ProcedureType::Activation:
            return at::factory(at::AT::CLIR_ENABLE);
        case ProcedureType::Interrogation:
            return at::factory(at::AT::CLIR_GET);
        case ProcedureType::Registration:
        case ProcedureType::Erasure:
            // not supported
            break;
        }
        return at::Cmd("");
    }

    void ClirRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    auto ClirRequest::isValid() const noexcept -> bool
    {
        return procedureType != ProcedureType::Registration && procedureType != ProcedureType::Erasure;
    }

} // namespace cellular
