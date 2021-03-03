// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Result.hpp>
#include <at/Commands.hpp>
#include <at/ATFactory.hpp>

#include "service-cellular/requests/ImeiRequest.hpp"

namespace cellular
{
    at::Cmd ImeiRequest::command()
    {
        return at::Cmd(at::factory(at::AT::GET_IMEI));
    }

    void ImeiRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    std::unique_ptr<ImeiRequest> ImeiRequest::create(const std::string &data, GroupMatch)
    {
        return std::make_unique<ImeiRequest>(data);
    }

}; // namespace cellular
