// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>

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

    auto ClirRequest::command() -> std::string
    {
        switch (procedureType) {
        case ProcedureType::Deactivation:
            return std::string(at::factory(at::AT::CLIR_DISABLE));
            break;
        case ProcedureType::Activation:
            return std::string(at::factory(at::AT::CLIR_ENABLE));
            break;
        case ProcedureType::Interrogation:
            return std::string(at::factory(at::AT::CLIR_GET));
            break;
        case ProcedureType::Registration:
            return std::string();
            break;
        case ProcedureType::Erasure:
            return std::string();
            break;
        }
        return std::string();
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
