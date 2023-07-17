// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>

namespace serial_number_parser
{
    constexpr auto grayColor     = "gray";
    constexpr auto blackColor    = "black";
    constexpr auto unknownColour = "unknown";

    constexpr auto unknownSerialNumber = "0000000000000";

    constexpr auto firstVersion   = 1;
    constexpr auto secondVersion  = 2;
    constexpr auto unknownVersion = 0;

    using SerialNumber    = std::string;
    using CaseColor       = std::string;
    using VersionNumber   = unsigned;
    using VersionMetadata = std::pair<CaseColor, VersionNumber>;
    using DeviceMetadata  = std::pair<SerialNumber, VersionMetadata>;
} // namespace serial_number_parser
