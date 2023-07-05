// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>

namespace serial_number_parser
{
    /* Serial number and device version */
    using VersionMetadata = std::pair<std::string, unsigned>;
    using DeviceMetadata  = std::pair<std::string, VersionMetadata>;

    DeviceMetadata getDeviceMetadata();
} // namespace serial_number_parser
