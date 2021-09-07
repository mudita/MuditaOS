// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ucs2/UCS2.hpp>

#include <string>

namespace cellular::internal::sms
{
    class SMSPartsHandler
    {
      public:
        explicit SMSPartsHandler(const std::string &rawMessage);

        bool isPartsCountAboveLimit() const;

        bool isSinglePartSMS() const;

        unsigned getPartsCount() const;

        const std::string getNextSmsPart();

      private:
        const UTF8 rawMessageUTF;
        unsigned partsCount;
        size_t lastIndex;
    };

} // namespace cellular::internal::sms
