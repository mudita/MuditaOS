// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/SystemManager/SystemManagerCommon.hpp>

namespace sys
{
    class SystemManager : public SystemManagerCommon
    {
      public:
        explicit SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators);
    };
} // namespace sys
