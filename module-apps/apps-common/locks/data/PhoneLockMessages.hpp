// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

namespace locks
{
    class UnlockPhone : public sys::DataMessage
    {};

    class CancelUnlockPhone : public sys::DataMessage
    {};

    class UnlockedPhone : public sys::DataMessage
    {};

    class LockPhone : public sys::DataMessage
    {};

    class LockedPhone : public sys::DataMessage
    {};

    class EnablePhoneLock : public sys::DataMessage
    {};

    class DisablePhoneLock : public sys::DataMessage
    {};

    class ChangePhoneLock : public sys::DataMessage
    {};

    class SetPhoneLock : public sys::DataMessage
    {};

    class SetConfirmedPhoneLock : public sys::DataMessage
    {};

    class SkipSetPhoneLock : public sys::DataMessage
    {};

    class SkippedSetPhoneLock : public sys::DataMessage
    {};

    class UnLockPhoneInput : public sys::DataMessage
    {
      private:
        std::vector<unsigned int> inputData;

      public:
        explicit UnLockPhoneInput(std::vector<unsigned int> inputData) : inputData(std::move(inputData))
        {}

        [[nodiscard]] auto getInputData() const noexcept
        {
            return inputData;
        }
    };

    class PhoneLockTimeUpdate : public sys::DataMessage
    {
      private:
        std::string formattedTime;

      public:
        explicit PhoneLockTimeUpdate(std::string formattedTime) : formattedTime(std::move(formattedTime))
        {}

        [[nodiscard]] const auto &getTime() const noexcept
        {
            return formattedTime;
        }
    };

    class ExternalUnLockPhone : public UnLockPhoneInput
    {
      public:
        explicit ExternalUnLockPhone(std::vector<unsigned int> inputData) : UnLockPhoneInput(std::move(inputData))
        {}
    };

    class ExternalPhoneLockAvailabilityChange : public sys::DataMessage
    {
      private:
        bool value = true;

      public:
        explicit ExternalPhoneLockAvailabilityChange(bool value) : value(value)
        {}

        [[nodiscard]] auto getAvailability() const noexcept
        {
            return value;
        }
    };

    class ExternalUnLockPhoneInfo : public sys::DataMessage
    {
      private:
        unsigned int attemptsLeft;

      public:
        explicit ExternalUnLockPhoneInfo(unsigned int attemptsLeft) : attemptsLeft(attemptsLeft)
        {}

        [[nodiscard]] auto getAttemptsLeft() const noexcept
        {
            return attemptsLeft;
        }
    };
} // namespace locks
