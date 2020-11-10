// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <functional>
#include <string>
#include <memory>

#include "CallRequest.hpp"
#include <at/Result.hpp>
#include <at/Commands.hpp>

namespace call_request
{

    std::string IMEIRequest::command(void)
    {
        return std::string(at::factory(at::AT::GET_IMEI));
    }

    std::unique_ptr<IMEIRequest> IMEIRequest::create(const std::string &data)
    {
        return std::make_unique<IMEIRequest>(data);
    }

    std::string USSDRequest::command(void)
    {
        return std::string(at::factory(at::AT::CUSD_SEND) + request + ",15\r");
    }

    std::unique_ptr<USSDRequest> USSDRequest::create(const std::string &data)
    {
        return std::make_unique<USSDRequest>(data);
    }

    std::string CallRequest::command(void)
    {
        return std::string(at::factory(at::AT::ATD) + request + ";\r");
    }

}; // namespace call_request
