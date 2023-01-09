// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TetheringHandler.hpp"

#include <module-cellular/at/cmd/QCFGUsbnet.hpp>

namespace cellular::service
{

    auto TetheringHandler::isRndisEnabled() -> bool
    {
        auto result = onIsRndisEnabled();
        if (result.has_value()) {
            return result == at::qcfg::usbnet::Net::RNDIS;
        }
        return false;
    }

    auto TetheringHandler::configure() -> bool
    {
        if (isRndisEnabled()) {
            return true;
        }
        onEnableRndis();
        return false;
    }

    auto TetheringHandler::disable() -> bool
    {
        onSetPassthrough(false);
        if (onEnableUrc() && onReadMessages()) {
            return true;
        }
        LOG_FATAL("Tethering disabling failed");
        return false;
    }

    auto TetheringHandler::enable() -> bool
    {
        onSetPassthrough(true);
        if (onDisableUrc()) {
            return true;
        }
        LOG_FATAL("Tethering enabling failed");
        return false;
    }
} // namespace cellular::service
