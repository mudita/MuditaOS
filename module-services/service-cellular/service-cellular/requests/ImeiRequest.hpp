// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>

#include "service-cellular/RequestHandler.hpp"
#include "service-cellular/requests/Request.hpp"

namespace cellular
{
    constexpr inline auto ImeiRegex = "(\\*#06#)";

    class ImeiRequest : public Request
    {
      public:
        ImeiRequest(const std::string &data) : Request(data){};
        std::string command() final;

        static std::unique_ptr<ImeiRequest> create(const std::string &data, GroupMatch);
        void handle(RequestHandler &h, at::Result &result) final;
    };
} // namespace cellular
