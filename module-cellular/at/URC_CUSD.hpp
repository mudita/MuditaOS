// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC_Any.hpp"

namespace at::urc
{

    struct CUSD : public Any
    {

        const std::string urc_name = "+CUSD";

        CUSD(const std::string &val);
        ~CUSD() override = default;

        enum class Tokens
        {
            Status,
            Response,
            DCS
        };

        auto what() -> std::string final;
        bool isActionNeeded(void);
        std::string message(void);
    };
} // namespace at::urc
