// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "ServiceEinkApi.hpp"

#include "internal/StaticData.hpp"

namespace service::eink::api
{
    [[nodiscard]] auto isInvertedModeEnabled() noexcept -> bool
    {
        return internal::StaticData::get().isInvertedModeEnabled();
    }
} // namespace service::eink::api
