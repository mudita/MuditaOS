// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "StaticData.hpp"

namespace service::eink::internal
{
    StaticData &StaticData::get()
    {
        static StaticData instance;
        return instance;
    }

    void StaticData::setInvertedMode(bool enabled) noexcept
    {
        invertedEnabled = enabled;
    }

    auto StaticData::isInvertedModeEnabled() const noexcept -> bool
    {
        return invertedEnabled;
    }
} // namespace service::eink::internal
