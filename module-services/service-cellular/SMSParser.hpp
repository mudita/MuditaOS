// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <utf8/UTF8.hpp>

#include <cstdint>
#include <string>

namespace SMSParser
{
    bool parse(const std::string *smsData);
    std::string getMessage();
    UTF8 getNumber();
    time_t getTime();
}; // namespace SMSParser
