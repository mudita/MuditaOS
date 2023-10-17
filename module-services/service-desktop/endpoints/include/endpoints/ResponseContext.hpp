// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <json11.hpp>
#include "HttpEnums.hpp"

namespace sdesktop::endpoints
{
    struct ResponseContext
    {
        http::Code status = http::Code::OK;
        json11::Json body = json11::Json();
    };
} // namespace sdesktop::endpoints
