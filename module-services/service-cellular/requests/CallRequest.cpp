// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Result.hpp>
#include <at/Commands.hpp>

#include "service-cellular/requests/CallRequest.hpp"
#include <at/ATFactory.hpp>

namespace cellular
{
    at::Cmd CallRequest::command()
    {
        auto cmd = at::factory(at::AT::ATD);
        return cmd + request + ";";
    }

    void CallRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
}; // namespace cellular
