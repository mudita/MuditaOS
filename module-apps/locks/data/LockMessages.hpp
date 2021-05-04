// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

namespace locks
{
    class UnlockPhone : public sys::DataMessage
    {
      public:
        UnlockPhone() : DataMessage{}
        {}
    };

    class CancelUnlockPhone : public sys::DataMessage
    {
      public:
        CancelUnlockPhone() : DataMessage{}
        {}
    };

    class UnlockedPhone : public sys::DataMessage
    {
      public:
        UnlockedPhone() : DataMessage{}
        {}
    };

    class LockPhone : public sys::DataMessage
    {
      public:
        LockPhone() : DataMessage{}
        {}
    };

    class UnLockPhoneInput : public sys::DataMessage
    {
      private:
        std::vector<unsigned int> inputData;

      public:
        explicit UnLockPhoneInput(std::vector<unsigned int> inputData) : DataMessage{}, inputData(std::move(inputData))
        {}

        [[nodiscard]] auto getInputData() const noexcept
        {
            return inputData;
        }
    };
} // namespace lock
