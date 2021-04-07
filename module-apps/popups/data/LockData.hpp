// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include "gui/SwitchData.hpp"
#include "popups/lock-widgets/PinLock.hpp"

namespace gui
{

    // class template that stores information that was sent along with switch message
    class LockData : public gui::SwitchData
    {
        std::string previousApplication;
        std::unique_ptr<PinLock> lock;

      public:
        LockData(std::unique_ptr<PinLock> &&lock) : SwitchData(), lock(std::move(lock))
        {
            description = "LockPhoneData";
        }

        virtual ~LockData(){};

        void setPrevApplication(const std::string &prevApp)
        {
            previousApplication = prevApp;
        };
        [[nodiscard]] const std::string &getPreviousApplication()
        {
            return previousApplication;
        };

        [[nodiscard]] std::unique_ptr<PinLock> getLock()
        {
            return std::make_unique<PinLock>(*lock.get());
        }
    };

} // namespace gui
