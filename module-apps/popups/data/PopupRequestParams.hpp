// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"

#include <service-appmgr/Actions.hpp>
#include <module-sys/PhoneModes/Common.hpp>
#include <module-audio/Audio/AudioCommon.hpp>
#include <module-apps/popups/lock-widgets/Lock.hpp>

namespace gui
{
    class PopupRequestParams : public app::manager::actions::ActionParams
    {
      public:
        explicit PopupRequestParams(gui::popup::ID popupId)
            : app::manager::actions::ActionParams{"Popup request parameters"}, popupId{popupId}
        {}

        [[nodiscard]] auto getPopupId() const noexcept
        {
            return popupId;
        }

      private:
        gui::popup::ID popupId;
    };

    class PhoneUnlockInputRequestParams : public PopupRequestParams
    {
      public:
        explicit PhoneUnlockInputRequestParams(gui::popup::ID popupId, lock::Lock *lock)
            : PopupRequestParams{popupId}, lock{lock}
        {}

        [[nodiscard]] auto getLock() const noexcept
        {
            return lock;
        }

      private:
        lock::Lock *lock;
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
