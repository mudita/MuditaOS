// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>
#include <string>

namespace pdu
{

    struct MmsNotification
    {
        MmsNotification(std::string &&fromAddress, std::string &&contentLocation)
            : fromAddress(std::move(fromAddress)), contentLocation(std::move(contentLocation))
        {}

        std::string fromAddress;
        std::string contentLocation;
    };

    std::optional<MmsNotification> parse(std::string const &message);

} // namespace pdu
