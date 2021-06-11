// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-cellular/api/static.hpp>

#include "static-api.hpp"

#include <memory>

namespace cellular::api
{
    using namespace internal;
    SimSlot currentSimSlot()
    {
        return static_data::get()->getSimSlot();
    }

    SimState currentSimState()
    {
        return static_data::get()->getSimState();
    }

    TrayState trayState()
    {
        return static_data::get()->getTrayState();
    }

    std::string ownNumber()
    {
        return static_data::get()->getOwnNumber();
    }

} // namespace cellular::api

namespace cellular::internal
{
    static_data *static_data::get()
    {
        static std::unique_ptr<static_data> instance(new static_data);
        return instance.get();
    }

    api::SimSlot static_data::getSimSlot() const
    {
        return simSlot;
    }

    void static_data::setSimSlot(api::SimSlot slot)
    {
        if (slot != simSlot) {
            simSlot = slot;
            if (onSimSlot)
                onSimSlot(slot);
        }
    }

    api::SimState static_data::getSimState() const
    {
        return simState;
    }

    void static_data::setSimState(api::SimState state)
    {
        if (state != simState) {
            simState = state;
            if (onSimState)
                onSimState(state);
        }
    }

    api::TrayState static_data::getTrayState() const
    {
        return trayState;
    }

    void static_data::setTrayState(api::TrayState state)
    {
        if (state != trayState) {
            trayState = state;
            if (onTrayState)
                onTrayState(state);
        }
    }

    std::string static_data::getOwnNumber() const
    {
        cpp_freertos::LockGuard lock(mutex);
        return ownNumber;
    }

    void static_data::setOwnNumber(const std::string &number)
    {
        if (number != getOwnNumber()) {
            {
                cpp_freertos::LockGuard lock(mutex);
                ownNumber = number;
            }
            if (onOwnNumber)
                onOwnNumber(number);
        }
    }

} // namespace cellular::internal
