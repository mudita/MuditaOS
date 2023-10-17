// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <at/Commands.hpp>
#include <Utils.hpp>
#include <at/ATFactory.hpp>

#include "service-cellular/requests/ClipRequest.hpp"

namespace cellular
{

    std::unique_ptr<SupplementaryServicesRequest> ClipRequest::create(const std::string &serviceCode,
                                                                      const std::string &data,
                                                                      GroupMatch matchGroups)
    {
        if (serviceCode == clipServiceCode) {
            return std::make_unique<ClipRequest>(data, matchGroups);
        }
        else {
            return nullptr;
        }
    }

    auto ClipRequest::command() -> at::Cmd
    {
        return isValid() ? at::factory(at::AT::CLIP_GET) : at::Cmd("");
    }

    auto ClipRequest::isValid() const noexcept -> bool
    {
        return procedureType == ProcedureType::Interrogation;
    }

    void ClipRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

} // namespace cellular
