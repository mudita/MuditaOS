// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "VolteCapabiltyHandlerCellularInterface.hpp"
#include "ImsiParserInterface.hpp"
#include "VolteAllowedListInterface.hpp"

#include <memory>

namespace cellular::service
{
    class VolteCapabilityHandler
    {
      public:
        VolteCapabilityHandler(std::unique_ptr<ImsiParserInteface> imsiParser,
                               std::unique_ptr<VolteAllowedListInterface> allowedList,
                               std::unique_ptr<VolteCapabilityCellularInterface> cellularInterface);
        /** Check if it is a possibility to enable VoLTE on current operator
         * @return true when VoLTE is allowed, false when not
         */
        auto isVolteAllowed(at::BaseChannel &channel) -> bool;

      private:
        std::unique_ptr<ImsiParserInteface> imsiParser;
        std::unique_ptr<VolteAllowedListInterface> allowedList;
        std::unique_ptr<VolteCapabilityCellularInterface> cellularInterface;
    };
} // namespace cellular::service
