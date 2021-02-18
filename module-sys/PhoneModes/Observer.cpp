// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Observer.hpp"

#include <Service/Service.hpp>

namespace sys::phone_modes
{
    void Observer::connect(Service *owner)
    {
        owner->connect(typeid(PhoneModeChanged), [this](sys::Message *request) -> sys::MessagePointer {
            return handlePhoneModeChange(static_cast<PhoneModeChanged *>(request));
        });
    }

    void Observer::subscribe(OnChangeCallback &&onChange,
                             OnCompleteCallback &&onComplete,
                             OnErrorCallback &&onError) noexcept
    {
        onChangeCallback   = std::move(onChange);
        onCompleteCallback = std::move(onComplete);
        onErrorCallback    = std::move(onError);
    }

    bool Observer::isInMode(PhoneMode mode) const noexcept
    {
        return phoneMode == mode;
    }

    PhoneMode Observer::getCurrentPhoneMode() const noexcept
    {
        return phoneMode;
    }

    bool Observer::isTetheringOn() const noexcept
    {
        return tetheringMode == Tethering::On;
    }

    sys::MessagePointer Observer::handlePhoneModeChange(PhoneModeChanged *message)
    {
        phoneMode     = message->getPhoneMode();
        tetheringMode = message->getTetheringMode();

        try {
            onPhoneModeChanged();
        }
        catch (const std::exception &) {
            return std::make_shared<PhoneModeChangeFailed>();
        }
        return std::make_shared<PhoneModeChangedSuccessfully>();
    }

    void Observer::onPhoneModeChanged()
    {
        if (!onChangeCallback) {
            LOG_WARN("No subscriber on phone mode change.");
            return;
        }

        try {
            onChangeCallback(phoneMode, tetheringMode);
            if (onCompleteCallback) {
                onCompleteCallback();
            }
        }
        catch (const std::exception &error) {
            if (onErrorCallback) {
                onErrorCallback(error);
            }
            throw;
        }
    }
} // namespace sys::phone_modes
