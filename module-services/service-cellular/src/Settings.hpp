// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/api/common.hpp>

#include <service-db/Settings.hpp>

#include <optional>

namespace cellular::service
{
    struct Settings : protected settings::Settings
    {
        explicit Settings(sys::Service *owner);

        void setSimSlot(api::SimSlot slot);
        std::optional<api::SimSlot> getSimSlot();
    };
} // namespace cellular::service
