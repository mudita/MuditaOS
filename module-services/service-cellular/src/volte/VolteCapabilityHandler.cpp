// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteCapabilityHandler.hpp"
#include <modem/BaseChannel.hpp>
#include <log/log.hpp>

namespace cellular::service
{
    VolteCapabilityHandler::VolteCapabilityHandler(std::unique_ptr<ImsiParserInteface> imsiParser,
                                                   std::unique_ptr<VolteAllowedListInterface> allowedList,
                                                   std::unique_ptr<VolteCapabilityCellularInterface> cellularInterface)
        : imsiParser(std::move(imsiParser)), allowedList(std::move(allowedList)),
          cellularInterface(std::move(cellularInterface))
    {}

    auto VolteCapabilityHandler::isVolteAllowed(at::BaseChannel &channel) -> bool
    {
        const auto imsi = cellularInterface->getImsi(channel);
        if (not imsi.has_value()) {
            LOG_ERROR("[VoLTE] failed to read IMSI - VoLTE not permitted");
            return false;
        }

        const auto operatorInfo = imsiParser->parse(imsi.value());
        if (not operatorInfo.has_value()) {
            LOG_ERROR("[VoLTE] failed to parse IMSI - VoLTE not permitted");
            return false;
        }

        return allowedList->isVolteAllowed(operatorInfo.value());
    }
} // namespace cellular::service
