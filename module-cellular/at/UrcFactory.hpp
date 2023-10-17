// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include "Urc.hpp"
#include <memory>

namespace at::urc
{
    class UrcFactory
    {
      public:
        /**
         * Instantiates concrete Urc class on a base of Urc message body.
         * @param urcMessage - Urc message to be parsed
         * @return pointer to Urc
         */
        static std::unique_ptr<Urc> Create(const std::string &urcMessage);
    };
} // namespace at::urc
