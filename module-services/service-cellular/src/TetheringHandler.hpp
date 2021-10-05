// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <functional>

namespace at
{
    class Cmd;
    class BaseChannel;
} // namespace at

namespace at::qcfg::usbnet
{
    enum class Net;
}
namespace cellular::service
{

    class TetheringHandler
    {
      public:
        /**
         * Reads current Rndis configuration
         * @return true if Rndis is enabled, false if Rndis is disabled
         */
        auto isRndisEnabled() -> bool;
        /**
         * It's called to configure Rndis mode
         * @return
         */
        auto configure() -> bool;
        /**
         * It's enabling tethering mode. Disables all URC and sets modem on passthrough via USB
         * @return true on succes, false on fail
         */
        auto enable() -> bool;
        /**
         * It's disabling tethering mode. Enables all URC, reads all unreaded messages and disables passtrough via USB
         * @return
         */
        auto disable() -> bool;

        /**
         * User callbacks
         */
        std::function<std::optional<at::qcfg::usbnet::Net>()> onIsRndisEnabled;
        std::function<bool()> onEnableRndis;
        std::function<bool()> onDisableUrc;
        std::function<bool()> onEnableUrc;
        std::function<bool()> onReadMessages;
        std::function<void(bool)> onSetPassthrough;
    };
} // namespace cellular::service
