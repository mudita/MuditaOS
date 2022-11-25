// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImsiParserUS.hpp"
#include <log/log.hpp>
#include <array>
#include <algorithm>
#include <stdexcept>

namespace cellular::service
{
    constexpr auto usMccCount = 7;
    const std::array<std::string, usMccCount> usMcc{"310", "311", "312", "313", "314", "315", "316"};

    auto ImsiParserUS::parse(const std::string &imsi) -> std::optional<OperatorInfo>
    {
        constexpr auto mccSize = 3;
        constexpr auto mncSize = 3;

        std::string mcc, mnc;
        try {
            mcc = imsi.substr(0, mccSize);
            mnc = imsi.substr(mccSize, mncSize);
        }
        catch (const std::out_of_range &e) {
            LOG_ERROR("[VoLTE] IMSI parsing error: %s", e.what());
            return std::nullopt;
        }

        if (std::find(std::begin(usMcc), std::end(usMcc), mcc) == std::end(usMcc)) {
            LOG_ERROR("[VoLTE] MCC not from USA");
            return std::nullopt;
        }

        return OperatorInfo(mcc, mnc);
    }
} // namespace cellular::service
