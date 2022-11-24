// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "VolteAllowedListInterface.hpp"
#include "OperatorInfo.hpp"

namespace cellular::service
{
    class VolteAllowedUSList : public VolteAllowedListInterface
    {
      public:
        auto isVolteAllowed(const OperatorInfo &operatorInfo) -> bool final;
    };
} // namespace cellular::service
