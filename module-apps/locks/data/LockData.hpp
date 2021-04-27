// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include "gui/SwitchData.hpp"
#include "locks/widgets/Lock.hpp"

namespace locks
{

    // class template that stores information that was sent along with switch message
    class LockData : public gui::SwitchData
    {
        Lock lock;

      public:
        explicit LockData(Lock lock) : SwitchData(), lock(std::move(lock))
        {
            description = "LockPhoneData";
        }

        [[nodiscard]] const Lock &getLock() const noexcept
        {
            return lock;
        }
    };

} // namespace lock
