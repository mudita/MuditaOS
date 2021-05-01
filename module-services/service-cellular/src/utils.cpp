// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "utils.hpp"

#include <algorithm>

namespace cellular::utils
{
    std::string pinToString(const cellular::api::PassCode &v)
    {
        std::string buf;
        std::transform(v.begin(), v.end(), std::back_inserter(buf), [](auto &&c) { return '0' + c; });
        return buf;
    }
} // namespace cellular::utils
