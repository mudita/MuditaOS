// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Common.hpp"
#include <optional>

namespace serial_number_parser
{
    bool isOldSerialNumberFormat(const std::string &serialNumber);
    std::optional<VersionMetadata> getDeviceVersionMetadata(const std::string &serialNumber);
} // namespace serial_number_parser
