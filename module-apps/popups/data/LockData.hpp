// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include "gui/SwitchData.hpp"
#include "popups/lock-widgets/Lock.hpp"

namespace lock
{

    // class template that stores information that was sent along with switch message
    class LockData : public gui::SwitchData
    {
        std::unique_ptr<Lock> lock;

      public:
        LockData(std::unique_ptr<Lock> &&lock) : SwitchData(), lock(std::move(lock))
        {
            description = "LockPhoneData";
        }

        virtual ~LockData(){};

        [[nodiscard]] std::unique_ptr<Lock> getLock()
        {
            return std::make_unique<Lock>(*lock.get());
        }
    };

} // namespace gui
