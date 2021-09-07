// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>
#include <chrono>

#include <at/Result.hpp>
#include <at/Commands.hpp>

#include "service-cellular/requests/UssdRequest.hpp"
#include <at/ATFactory.hpp>

namespace cellular
{
    auto UssdRequest::command() -> at::Cmd
    {
        using namespace std::chrono_literals;

        const auto timeout = 120000ms;
        return at::Cmd(at::factory(at::AT::CUSD_SEND) + request + ",15", timeout);
    }

    auto UssdRequest::create(const std::string &data, GroupMatch) -> std::unique_ptr<UssdRequest>
    {
        return std::make_unique<UssdRequest>(data);
    }

    void UssdRequest::handle(RequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
}; // namespace cellular
