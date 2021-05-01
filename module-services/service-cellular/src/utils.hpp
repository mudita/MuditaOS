// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/api/common.hpp>

#include <string>

namespace cellular::utils
{
    std::string pinToString(const cellular::api::PassCode &v);
}
