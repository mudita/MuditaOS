// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <service-cellular-api>

namespace locks
{
    class UnLockSimInput : public sys::DataMessage
    {
      private:
        std::vector<unsigned int> inputData;

      public:
        explicit UnLockSimInput(std::vector<unsigned int> inputData) : DataMessage{}, inputData(std::move(inputData))
        {}

        [[nodiscard]] auto getInputData() const noexcept
        {
            return inputData;
        }
    };

    class SetSim : public sys::DataMessage
    {
      private:
        cellular::api::SimSlot simSlot;

      public:
        explicit SetSim(cellular::api::SimSlot simSlot) : DataMessage{}, simSlot(simSlot)
        {}

        [[nodiscard]] auto getSimSlot() const noexcept
        {
            return simSlot;
        }
    };

    class ChangeSimPin : public sys::DataMessage
    {};

    class EnableSimPin : public sys::DataMessage
    {};

    class DisableSimPin : public sys::DataMessage
    {};

} // namespace locks
