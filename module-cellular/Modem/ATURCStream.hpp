// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <list>

namespace at
{
    constexpr auto urcStart = "\r\n";

    class ATURCStream
    {
      private:
        std::string atBuffer   = {};
        bool urcBeginFound     = false;
        bool lookForNextURC    = false;
        bool openQuotationMark = false;
        size_t posMark         = 0;
        std::list<std::string> urcList;

        uint32_t lastWriteTime;

        bool search();
        void tryParse();
        void fuse();
        bool lookForNext();
        bool checkQuotation();

      public:
        bool write(const std::string &buffer);
        /**
         * Get ready URC as vector and clear
         */
        [[nodiscard]] std::list<std::string> getURCList()
        {
            return std::move(urcList);
        }
    };
} // namespace at
