// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "VolteCapabiltyHandlerCellularInterface.hpp"

#include <optional>
#include <string>

namespace at
{
    class BaseChannel;
} // namespace at

namespace cellular::service
{
    struct VolteCapabilityCellular : VolteCapabilityCellularInterface
    {
        auto getImsi(at::BaseChannel &) -> std::optional<std::string> final;
    };
} // namespace cellular::service
