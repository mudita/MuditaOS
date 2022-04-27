// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CommandData.hpp"

namespace bluetooth
{

    class NetworkStatusData : public CommandData
    {
      public:
        explicit NetworkStatusData(const Store::Network::Status &status);
        auto getData() -> DataVariant override;

      private:
        Store::Network::Status status;
    };

} // namespace bluetooth
