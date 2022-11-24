// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteCapabilityHandlerCellular.hpp"
#include <modem/mux/CellularMux.h>

namespace cellular::service
{
    void VolteCapabilityCellular::setChannel(at::BaseChannel *channel)
    {
        this->channel = channel;
    }

    auto VolteCapabilityCellular::getImsi() -> std::optional<std::string>
    {
        if (channel == nullptr) {
            LOG_ERROR("No channel provided. Request ignored");
            return std::nullopt;
        }

        auto result = channel->cmd(at::AT::CIMI);
        if (not result) {
            LOG_ERROR("Failed to read IMSI, disable VoLTE.");
            return std::nullopt;
        }

        return result.response[0];
    }
} // namespace cellular::service
