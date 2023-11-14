// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <algorithm>
#include <functional>
#include <map>

namespace utils::anonymize
{
    // Determine how many characters to leave at the end of anonymized substring
    constexpr auto SIGNS_TO_LEAVE_FOR_PIN_AND_PUK       = 0;
    constexpr auto SIGNS_TO_LEAVE_FOR_PHONE_NUMBERS     = 2;
    constexpr auto SIGNS_TO_LEAVE_FOR_NET_PROVIDER_NAME = 1;

    // Basic function to anonymize provided string
    std::string anonymizeInQuotationMarks(const std::string &textToAnonymize, std::size_t singsToLeaveAtEnd = 0);
    std::string anonymizeNumbers(const std::string &textToAnonymize, std::size_t singsToLeaveAtEnd = 0);

    // To anonymize logs from cellular
    std::string anonymizeCellularIfNecessary(const std::string &text);

} // namespace utils::anonymize
