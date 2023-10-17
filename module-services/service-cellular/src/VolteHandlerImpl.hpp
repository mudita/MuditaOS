// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "VolteHandler.hpp"
#include <at/response.hpp>

namespace cellular::internal
{
    using namespace cellular::service;

    struct ModemResponseParserImpl
    {
        bool operator()(QcfgImsResult const &, bool) const;
    };
} // namespace cellular::internal
