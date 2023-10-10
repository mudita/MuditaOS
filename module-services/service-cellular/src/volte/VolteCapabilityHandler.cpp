// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteCapabilityHandler.hpp"
#include <modem/BaseChannel.hpp>
#include <log/log.hpp>

namespace cellular::service
{
    VolteCapabilityHandler::VolteCapabilityHandler(std::unique_ptr<VolteAllowedListInterface> allowedList,
                                                   std::unique_ptr<VolteCapabilityCellularInterface> cellularInterface)
        : allowedList(std::move(allowedList)), cellularInterface(std::move(cellularInterface))
    {}

    auto VolteCapabilityHandler::isVolteAllowed(at::BaseChannel &channel) -> bool
    {
        const auto &imsi = cellularInterface->getImsi(channel);
        if (!imsi.has_value()) {
            LOG_ERROR("VoLTE not permitted - failed to read IMSI");
            return false;
        }

        return allowedList->isVolteAllowed(imsi.value());
    }

    auto VolteCapabilityHandler::getSupportStatus(at::BaseChannel &channel) -> ImsiParser::SupportStatus
    {
        const auto &imsi = cellularInterface->getImsi(channel);
        if (!imsi.has_value()) {
            LOG_ERROR("VoLTE not permitted - failed to read IMSI");
            return ImsiParser::SupportStatus::Unsupported;
        }

        return allowedList->getSupportStatus(imsi.value());
    }
} // namespace cellular::service
