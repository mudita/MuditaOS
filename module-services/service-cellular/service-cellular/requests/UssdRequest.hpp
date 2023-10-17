// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>

#include "service-cellular/RequestHandler.hpp"
#include "service-cellular/requests/Request.hpp"

namespace cellular
{
    constexpr inline auto UssdRegex = "^[\\*\\#].*[\\#]$";

    class UssdRequest : public ConfirmingRequest
    {
      public:
        explicit UssdRequest(const std::string &data) : ConfirmingRequest(data){};
        auto command() -> at::Cmd final;

        static auto create(const std::string &data, GroupMatch) -> std::unique_ptr<UssdRequest>;
        void handle(RequestHandler &h, at::Result &result) final;
    };
} // namespace cellular
