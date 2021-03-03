// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        std::string body;
        switch (procedureType) {
        case ProcedureType::Deactivation:
            body = at::factory(at::AT::COLP_DISABLE);
            break;
        case ProcedureType::Activation:
            body = at::factory(at::AT::COLP_ENABLE);
            break;
        case ProcedureType::Interrogation:
            body = at::factory(at::AT::COLP_GET);
            break;
        case ProcedureType::Registration:
            // not supported
            break;
        case ProcedureType::Erasure:
            // not supported
            break;
        }
        return at::Cmd(body, at::default_long_doc_timeout);
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
