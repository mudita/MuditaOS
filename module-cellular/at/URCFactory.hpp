// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

namespace at::urc
{
    class URCFactory
    {
      public:
        /**
         * Instantiates concrete URC class on a base of URC message body.
         * @param urcMessage - URC message to be parsed
         * @return pointer to URC
         */
        static std::unique_ptr<URC> Create(const std::string &urcMessage);
    };
} // namespace at::urc
