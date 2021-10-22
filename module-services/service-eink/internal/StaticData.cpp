// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StaticData.hpp"

namespace service::eink::internal
{
    StaticData &StaticData::get()
    {
        static StaticData instance;
        return instance;
    }

    void StaticData::enableDarkMode() noexcept
    {
        darkModeEnabled = true;
    }

    void StaticData::disableDarkMode() noexcept
    {
        darkModeEnabled = false;
    }

    auto StaticData::isDarkModeEnabled() const noexcept -> bool
    {
        return darkModeEnabled;
    }
} // namespace service::eink::internal
