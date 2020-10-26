// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>

namespace app
{
    enum class StartupStatus
    {
        Success,
        Failure
    };

    struct StartInBackground
    {
        bool value = false;

        explicit operator bool() const noexcept
        {
            return value;
        }
    };

    namespace manager
    {
        using ApplicationName = std::string;
        using ActionId        = int;
        using ActionFilter    = std::vector<ActionId>;
    } // namespace manager
} // namespace app
