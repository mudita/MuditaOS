// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/api/common.hpp>
#include <mutex.hpp>
#include <functional>

namespace cellular::internal
{
    class static_data
    {
        static_data() = default;

        mutable cpp_freertos::MutexStandard mutex;

        api::SimSlot simSlot;
        api::SimState simState = api::SimState::Invalid;
        api::TrayState trayState;
        std::string ownNumber = {};

      public:
        static_data(const static_data &) = delete;
        static_data &operator=(const static_data &) = delete;

        static static_data *get();

        api::SimSlot getSimSlot() const;
        void setSimSlot(api::SimSlot slot);
        std::function<void(api::SimSlot)> onSimSlot;

        api::SimState getSimState() const;
        void setSimState(api::SimState state);
        std::function<void(api::SimState)> onSimState;

        api::TrayState getTrayState() const;
        void setTrayState(api::TrayState state);
        std::function<void(api::TrayState)> onTrayState;

        std::string getOwnNumber() const;
        void setOwnNumber(const std::string &number);
        std::function<void(const std::string &)> onOwnNumber;
    };
} // namespace cellular::internal
