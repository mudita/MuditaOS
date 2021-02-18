// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SMSRecord.hpp"
#include "service-cellular/ServiceCellular.hpp"

namespace cellular::internal
{

    class SMSSend
    {
      protected:
        ServiceCellular &cellular;

      public:
        explicit SMSSend(ServiceCellular &p) : cellular(p)
        {}
        [[nodiscard]] auto send(SMSRecord &record) -> bool;
    };
} // namespace cellular::internal
