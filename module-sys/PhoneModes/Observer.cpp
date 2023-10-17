// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <PhoneModes/Observer.hpp>

#include <Service/Service.hpp>
#include <log/log.hpp>

namespace sys::phone_modes
{
    void Observer::connect(Service *owner)
    {
        owner->connect(typeid(PhoneModeChanged), [this](sys::Message *request) -> sys::MessagePointer {
            return handlePhoneModeChange(static_cast<PhoneModeChanged *>(request));
        });
        owner->connect(typeid(TetheringChanged), [this](sys::Message *request) -> sys::MessagePointer {
            return handleTetheringChange(static_cast<TetheringChanged *>(request));
        });
    }

    void Observer::subscribe(OnPhoneModeChangedCallback &&onChange) noexcept
    {
        onPhoneModeChangedCallback = std::move(onChange);
    }

    void Observer::subscribe(OnTetheringChangedCallback &&onChange) noexcept
    {
        onTetheringChangedCallback = std::move(onChange);
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
        phoneMode = message->getPhoneMode();
        if (!onPhoneModeChangedCallback) {
            LOG_WARN("No subscriber on phone mode change.");
            return MessageNone{};
        }

        try {
            onPhoneModeChangedCallback(phoneMode);
        }
        catch (const std::exception &) {
            return std::make_shared<ChangeFailed>();
        }
        return std::make_shared<ChangedSuccessfully>();
    }

    sys::MessagePointer Observer::handleTetheringChange(TetheringChanged *message)
    {
        tetheringMode = message->getTetheringMode();
        if (!onTetheringChangedCallback) {
            LOG_WARN("No subscriber on phone mode change.");
            return MessageNone{};
        }

        try {
            onTetheringChangedCallback(tetheringMode);
        }
        catch (const std::exception &) {
            return std::make_shared<ChangeFailed>();
        }
        return std::make_shared<ChangedSuccessfully>();
    }
} // namespace sys::phone_modes
