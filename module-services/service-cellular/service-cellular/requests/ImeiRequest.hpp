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
        auto command() -> at::Cmd final;

        static auto create(const std::string &data, GroupMatch) -> std::unique_ptr<ImeiRequest>;
        void handle(RequestHandler &h, at::Result &result) final;
    };
} // namespace cellular
