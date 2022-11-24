// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "VolteCapabiltyHandlerCellularInterface.hpp"
#include "ImsiParserInterface.hpp"
#include "VolteAllowedListInterface.hpp"

#include <memory>
#include <vector>

namespace at
{
    class Result;
    class BaseChannel;
} // namespace at

namespace cellular::service
{
    class VolteCapabilityHandler
    {
      public:
        VolteCapabilityHandler(std::unique_ptr<ImsiParserInteface> imsiParser,
                               std::unique_ptr<VolteAllowedListInterface> allowedList,
                               std::unique_ptr<VolteCapabilityCellularInterface> cellularInterface);
        /** Set AT command channel
         * \param channel channel (or nullptr to block communication):
         */
        void setChannel(at::BaseChannel *channel);
        /** Check if it is a possibility to enable VoLTE on current operator
         * @return true when VoLTE is allowed, false when not
         */
        auto isVolteAllowed() -> bool;

      private:
        std::unique_ptr<ImsiParserInteface> imsiParser;
        std::unique_ptr<VolteAllowedListInterface> allowedList;
        std::unique_ptr<VolteCapabilityCellularInterface> cellularInterface;

        auto isCellularInterfaceReady() -> bool;
    };
} // namespace cellular::service
