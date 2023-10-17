// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteCapabilityHandlerCellular.hpp"
#include <modem/mux/CellularMux.h>
#include <module-cellular/modem/BaseChannel.hpp>

namespace cellular::service
{
    auto VolteCapabilityCellular::getImsi(at::BaseChannel &channel) -> std::optional<std::string>
    {
        auto result = channel.cmd(at::AT::CIMI);
        if (not result) {
            LOG_ERROR("[VoLTE] failed to read IMSI - will disable VoLTE");
            return std::nullopt;
        }

        return result.response[0];
    }
} // namespace cellular::service
