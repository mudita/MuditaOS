// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteCapabilityHandler.hpp"
#include <modem/mux/CellularMux.h>
#include <log/log.hpp>
namespace cellular::service
{
    VolteCapabilityHandler::VolteCapabilityHandler(std::unique_ptr<ImsiParserInteface> imsiParser,
                                                   std::unique_ptr<VolteAllowedListInterface> allowedList,
                                                   std::unique_ptr<VolteCapabilityCellularInterface> cellularInterface)
        : imsiParser(std::move(imsiParser)), allowedList(std::move(allowedList)),
          cellularInterface(std::move(cellularInterface))
    {}

    void VolteCapabilityHandler::setChannel(at::BaseChannel *channel)
    {
        if (cellularInterface.get() != nullptr) {
            cellularInterface->setChannel(channel);
        }
    }

    auto VolteCapabilityHandler::isVolteAllowed() -> bool
    {
        const auto imsi = cellularInterface->getImsi();
        if (not imsi.has_value()) {
            LOG_ERROR("Failed to read IMSI, disable VoLTE.");
            return false;
        }

        const auto operatorInfo = imsiParser->parse(imsi.value());
        if (not operatorInfo.has_value()) {
            LOG_ERROR("Failed to parse IMSI, disable VoLTE.");
            return false;
        }
        return allowedList->isVolteAllowed(operatorInfo.value());
    }

    auto VolteCapabilityHandler::isCellularInterfaceReady() -> bool
    {
        return cellularInterface.get() != nullptr;
    }
} // namespace cellular::service
