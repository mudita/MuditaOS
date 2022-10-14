// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"
#include "PopupRequestParamsBase.hpp"

#include <PhoneModes/Common.hpp>
#include <module-audio/Audio/AudioCommon.hpp>
#include <service-bluetooth/messages/Authenticate.hpp>
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
            : PopupRequestParams(
                  popupId, {popup::Disposition::Priority::Normal, popup::Disposition::WindowType::Popup, popupId}),
              lock{std::move(lock)}, simInputTypeAction(simInputTypeAction), errorCode(errorCode)
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

    class BluetoothAuthenticateRequestParams : public PopupRequestParams
    {
      private:
        Devicei device;
        bluetooth::AuthenticateType type;
        std::optional<unsigned long> pairingCode;

      public:
        explicit BluetoothAuthenticateRequestParams(const Devicei &dev,
                                                    bluetooth::AuthenticateType type,
                                                    std::optional<unsigned long> pairingCode = std::nullopt)
            : PopupRequestParams{gui::popup::ID::BluetoothAuthenticate}, device{dev}, type{type}, pairingCode{
                                                                                                      pairingCode}
        {}

        [[nodiscard]] auto getDevice() const noexcept
        {
            return device;
        }

        [[nodiscard]] auto getAuthenticateType() const noexcept
        {
            return type;
        }
        [[nodiscard]] auto getPairingCode() const noexcept
        {
            return pairingCode;
        }
    };

    class BluetoothInfoParams : public PopupRequestParams
    {
      private:
        Devicei device;
        bool succeed;
        std::string errorCode;

      public:
        explicit BluetoothInfoParams(const Devicei &dev, bool succeed, std::string errorCode = "")
            : PopupRequestParams{gui::popup::ID::BluetoothInfo}, device{dev}, succeed{succeed}, errorCode{errorCode}
        {}

        [[nodiscard]] auto getDevice() const noexcept
        {
            return device;
        }

        [[nodiscard]] auto isSucceed() const noexcept -> bool
        {
            return succeed;
        }
        [[nodiscard]] auto getErrorCode() const noexcept -> std::string
        {
            return errorCode;
        }
    };

    class VolumePopupRequestParams : public PopupRequestParams
    {
      public:
        VolumePopupRequestParams(audio::Volume volume,
                                 const audio::Context audioContext,
                                 audio::VolumeChangeRequestSource source)
            : PopupRequestParams{gui::popup::ID::Volume}, volume{volume}, audioContext{audioContext}, source{source}
        {}

        [[nodiscard]] auto getVolume() const noexcept
        {
            return volume;
        }

        [[nodiscard]] auto getAudioContext() const noexcept
        {
            return audioContext;
        }

        [[nodiscard]] auto getRequestSource() const noexcept
        {
            return source;
        }

      private:
        const audio::Volume volume;
        const audio::Context audioContext;
        const audio::VolumeChangeRequestSource source;
    };

} // namespace gui
