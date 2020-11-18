// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Result.hpp>
#include <at/Commands.hpp>

#include "service-cellular/requests/UssdRequest.hpp"

namespace cellular
{
    std::string UssdRequest::command()
    {
        return std::string(at::factory(at::AT::CUSD_SEND) + request + ",15");
    }

    std::unique_ptr<UssdRequest> UssdRequest::create(const std::string &data, GroupMatch)
    {
        return std::make_unique<UssdRequest>(data);
    }

    void UssdRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
}; // namespace cellular
