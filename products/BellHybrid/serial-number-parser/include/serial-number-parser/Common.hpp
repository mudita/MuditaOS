// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once
#include <string>

namespace serial_number_parser
{
    constexpr auto grayColor     = "gray";
    constexpr auto blackColor    = "black";
    constexpr auto unknownColour = "unknown";

    constexpr auto unknownSerialNumber = "0000000000000";

    constexpr auto unknownVersion   = "";
    constexpr auto firstVersion     = "1";
    constexpr auto secondVersion    = "2";
    constexpr auto secondProVersion = "2 Pro";

    using SerialNumber    = std::string;
    using CaseColour      = std::string;
    using VersionNumber   = std::string;
    using VersionMetadata = std::pair<CaseColour, VersionNumber>;
    using DeviceMetadata  = std::pair<SerialNumber, VersionMetadata>;
} // namespace serial_number_parser
