// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Result.hpp>
#include <at/Commands.hpp>

#include "service-cellular/requests/CallRequest.hpp"

namespace cellular
{
    std::string CallRequest::command()
    {
        return std::string(at::factory(at::AT::ATD) + request + ";");
    }

    void CallRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
}; // namespace cellular
