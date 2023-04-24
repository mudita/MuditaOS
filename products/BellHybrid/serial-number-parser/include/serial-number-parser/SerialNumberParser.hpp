// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>

namespace serial_number_parser
{
    std::pair<std::string, std::string> getDeviceMetadata();
} // namespace serial_number_parser
