// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

#include <ctime>

namespace locks
{
    class UnlockPhone : public sys::DataMessage
    {};

    class UnlockPhoneForMTP : public sys::DataMessage
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

    class UnlockPhoneInput : public sys::DataMessage
    {
      private:
        std::vector<unsigned> inputData;

      public:
        explicit UnlockPhoneInput(std::vector<unsigned> inputData) : inputData(std::move(inputData))
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
        bool isFirstData;

      public:
        explicit PhoneLockTimeUpdate(std::string formattedTime, bool isFirstData = true)
            : formattedTime(std::move(formattedTime)), isFirstData(isFirstData)
        {}

        [[nodiscard]] const auto &getTime() const noexcept
        {
            return formattedTime;
        }

        [[nodiscard]] const auto &getIsFirstData() const noexcept
        {
            return isFirstData;
        }
    };

    class NextPhoneUnlockAttemptLockTime : public sys::DataMessage
    {
      private:
        std::time_t nextPhoneUnlockAttemptLockTime;

      public:
        explicit NextPhoneUnlockAttemptLockTime(std::time_t nextPhoneUnlockAttemptLockTime)
            : nextPhoneUnlockAttemptLockTime(nextPhoneUnlockAttemptLockTime)
        {}

        [[nodiscard]] const auto &getTime() const noexcept
        {
            return nextPhoneUnlockAttemptLockTime;
        }
    };

    class ExternalUnlockPhone : public UnlockPhoneInput
    {
      public:
        explicit ExternalUnlockPhone(std::vector<unsigned> inputData) : UnlockPhoneInput(std::move(inputData))
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
} // namespace locks
