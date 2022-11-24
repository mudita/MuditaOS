// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "VolteCapabiltyHandlerCellularInterface.hpp"

#include <optional>
#include <string>

namespace at
{
    class Result;
    class BaseChannel;
} // namespace at

namespace cellular::service
{

    class VolteCapabilityCellular : public VolteCapabilityCellularInterface
    {
      public:
        void setChannel(at::BaseChannel *channel) final;
        auto getImsi() -> std::optional<std::string> final;

      private:
        at::BaseChannel *channel = nullptr;
    };
} // namespace cellular::service
