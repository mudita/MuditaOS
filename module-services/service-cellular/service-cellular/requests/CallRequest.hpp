// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>

#include "service-cellular/RequestHandler.hpp"
#include "service-cellular/requests/Request.hpp"

namespace cellular
{
    class CallRequest : public Request
    {
      public:
        CallRequest(const std::string &data) : Request(data){};
        at::Cmd command() final;
        std::string getNumber() const
        {
            return request;
        }
        static std::unique_ptr<CallRequest> create(const std::string &data, GroupMatch);
        void handle(RequestHandler &h, at::Result &result) final;
    };
}; // namespace cellular
