// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceEinkApi.hpp"

#include "internal/StaticData.hpp"

namespace service::eink::api
{
    [[nodiscard]] auto isDarkModeEnabled() noexcept -> bool
    {
        return internal::StaticData::get().isDarkModeEnabled();
    }
} // namespace service::eink::api
