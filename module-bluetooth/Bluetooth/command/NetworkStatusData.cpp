// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkStatusData.hpp"

namespace bluetooth
{
    NetworkStatusData::NetworkStatusData(const Store::Network::Status &status) : status(status)
    {}
    auto NetworkStatusData::getData() -> DataVariant
    {
        return status;
    }
} // namespace bluetooth
