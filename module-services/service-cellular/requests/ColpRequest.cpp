// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>
#include <at/ATFactory.hpp>

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

    auto ColpRequest::command() -> at::Cmd
    {
        switch (procedureType) {
        case ProcedureType::Deactivation:
            return at::factory(at::AT::COLP_DISABLE);
        case ProcedureType::Activation:
            return at::factory(at::AT::COLP_ENABLE);
        case ProcedureType::Interrogation:
            return at::factory(at::AT::COLP_GET);
        case ProcedureType::Registration:
        case ProcedureType::Erasure:
            // not supported
            break;
        }
        return at::Cmd("");
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
