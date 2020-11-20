// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>

#include "service-cellular/requests/ColpRequest.hpp"

namespace cellular
{

    std::unique_ptr<SupplementaryServicesRequest> ColpRequest::create(const std::string &serviceCode,
                                                                      const std::string &data,
                                                                      GroupMatch matchGroups)
    {
        if (serviceCode == colpServiceCode) {
            return std::make_unique<ColpRequest>(data, matchGroups);
        }
        else {
            return nullptr;
        }
    }

    auto ColpRequest::command() -> std::string
    {
        switch (procedureType) {
        case ProcedureType::Deactivation:
            return std::string(at::factory(at::AT::COLP_DISABLE));
        case ProcedureType::Activation:
            return std::string(at::factory(at::AT::COLP_ENABLE));
        case ProcedureType::Interrogation:
            return std::string(at::factory(at::AT::COLP_GET));
        case ProcedureType::Registration:
            // not supported
            return std::string();
        case ProcedureType::Erasure:
            // not supported
            return std::string();
        }
        return std::string();
    }

    void ColpRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    auto ColpRequest::isValid() const noexcept -> bool
    {
        return procedureType != ProcedureType::Registration && procedureType != ProcedureType::Erasure;
    }

} // namespace cellular
