// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <PhoneModes/Observer.hpp>

#include <Service/Service.hpp>

namespace sys::phone_modes
{
    namespace
    {
        template <typename F> void onChanged(const F &onChanged, const Observer::OnFinishedCallbacks &onFinished)
        {
            try {
                onChanged();
                if (onFinished.onComplete) {
                    onFinished.onComplete();
                }
            }
            catch (const std::exception &error) {
                if (onFinished.onError) {
                    onFinished.onError(error);
                }
                throw;
            }
        }
    } // namespace

    void Observer::connect(Service *owner)
    {
        owner->connect(typeid(PhoneModeChanged), [this](sys::Message *request) -> sys::MessagePointer {
            return handlePhoneModeChange(static_cast<PhoneModeChanged *>(request));
        });
        owner->connect(typeid(TetheringChanged), [this](sys::Message *request) -> sys::MessagePointer {
            return handleTetheringChange(static_cast<TetheringChanged *>(request));
        });
    }

    void Observer::subscribe(OnPhoneModeChangedCallback &&onChange,
                             OnCompleteCallback &&onComplete,
                             OnErrorCallback &&onError) noexcept
    {
        onPhoneModeChangedCallback           = std::move(onChange);
        onPhoneModeChangeFinished.onComplete = std::move(onComplete);
        onPhoneModeChangeFinished.onError    = std::move(onError);
    }

    void Observer::subscribe(OnTetheringChangedCallback &&onChange,
                             OnCompleteCallback &&onComplete,
                             OnErrorCallback &&onError) noexcept
    {
        onTetheringChangedCallback           = std::move(onChange);
        onTetheringChangeFinished.onComplete = std::move(onComplete);
        onTetheringChangeFinished.onError    = std::move(onError);
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
            onChanged([this]() { onPhoneModeChangedCallback(phoneMode); }, onPhoneModeChangeFinished);
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
            onChanged([this]() { onTetheringChangedCallback(tetheringMode); }, onTetheringChangeFinished);
        }
        catch (const std::exception &) {
            return std::make_shared<ChangeFailed>();
        }
        return std::make_shared<ChangedSuccessfully>();
    }
} // namespace sys::phone_modes
