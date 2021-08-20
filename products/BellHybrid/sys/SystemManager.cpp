// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <sys/SystemManager.hpp>

namespace sys
{
    SystemManager::SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators)
        : SystemManagerCommon(std::move(creators))
    {}
} // namespace sys
