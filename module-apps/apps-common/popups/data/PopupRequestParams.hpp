// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"
#include "PopupRequestParamsBase.hpp"

#include <PhoneModes/Common.hpp>
#include <module-audio/Audio/AudioCommon.hpp>
#include <locks/widgets/Lock.hpp>
#include <locks/data/LockData.hpp>

namespace gui
{
    class PhoneUnlockInputRequestParams : public PopupRequestParams
    {
      public:
        PhoneUnlockInputRequestParams(gui::popup::ID popupId,
                                      locks::Lock lock,
                                      locks::PhoneLockInputTypeAction phoneLockInputTypeAction)
            : PopupRequestParams{popupId}, lock{std::move(lock)}, phoneLockInputTypeAction(phoneLockInputTypeAction)
        {}

        [[nodiscard]] auto getLock() const noexcept
        {
            return lock;
        }

        [[nodiscard]] auto getPhoneLockInputTypeAction() const noexcept
        {
            return phoneLockInputTypeAction;
        }

      private:
        locks::Lock lock;
        locks::PhoneLockInputTypeAction phoneLockInputTypeAction;
    };

    class SimUnlockInputRequestParams : public PopupRequestParams
    {
      public:
        SimUnlockInputRequestParams(gui::popup::ID popupId,
                                    locks::Lock lock,
                                    locks::SimInputTypeAction simInputTypeAction,
                                    unsigned int errorCode = 0)
            : PopupRequestParams{popupId}, lock{std::move(lock)}, simInputTypeAction(simInputTypeAction),
              errorCode(errorCode)
        {}

        [[nodiscard]] auto getLock() const noexcept
        {
            return lock;
        }

        [[nodiscard]] auto getSimInputTypeAction() const noexcept
        {
            return simInputTypeAction;
        }

        [[nodiscard]] auto getErrorCode() const noexcept
        {
            return errorCode;
        }

      private:
        locks::Lock lock;
        locks::SimInputTypeAction simInputTypeAction;
        unsigned int errorCode;
    };

    class PhoneModePopupRequestParams : public PopupRequestParams
    {
      public:
        explicit PhoneModePopupRequestParams(sys::phone_modes::PhoneMode mode)
            : PopupRequestParams{gui::popup::ID::PhoneModes}, phoneMode{mode}
        {}

        [[nodiscard]] auto getPhoneMode() const noexcept
        {
            return phoneMode;
        }

      private:
        sys::phone_modes::PhoneMode phoneMode;
    };

    class VolumePopupRequestParams : public PopupRequestParams
    {
      public:
        VolumePopupRequestParams(audio::Volume volume, const audio::Context audioContext)
            : PopupRequestParams{gui::popup::ID::Volume}, volume{volume}, audioContext{audioContext}
        {}

        [[nodiscard]] auto getVolume() const noexcept
        {
            return volume;
        }

        [[nodiscard]] auto getAudioContext() const noexcept
        {
            return audioContext;
        }

      private:
        const audio::Volume volume;
        const audio::Context audioContext;
    };

} // namespace gui
