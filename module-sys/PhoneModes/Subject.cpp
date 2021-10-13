// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <PhoneModes/Subject.hpp>

#include <Service/Service.hpp>

#include <magic_enum.hpp>

#include <stdexcept>
#include <utility>

namespace sys::phone_modes
{
    Subject::Subject(Service *owner) : owner{owner}
    {
        if (owner == nullptr) {
            throw std::invalid_argument{"Subject's owner is invalid"};
        }
    }

    bool Subject::setMode(PhoneMode _phoneMode, Tethering _tetheringMode)
    {
        const bool phoneModeChanged = changePhoneMode(_phoneMode);
        if (phoneModeChanged) {
            notifyPhoneModeChange();
        }
        const bool tetheringChanged = changeTetheringMode(_tetheringMode);
        if (tetheringChanged) {
            notifyTetheringChange();
        }
        return phoneModeChanged || tetheringChanged;
    }

    bool Subject::setPhoneMode(PhoneMode mode)
    {
        const auto changed = changePhoneMode(mode);
        if (changed) {
            notifyPhoneModeChange();
        }
        return changed;
    }

    bool Subject::changePhoneMode(PhoneMode mode) noexcept
    {
        return std::exchange(phoneMode, mode) != mode;
    }

    bool Subject::setTetheringMode(Tethering mode)
    {
        const auto changed = changeTetheringMode(mode);
        if (changed) {
            notifyTetheringChange();
        }
        return changed;
    }

    bool Subject::changeTetheringMode(Tethering mode) noexcept
    {
        return std::exchange(tetheringMode, mode) != mode;
    }

    void Subject::notifyPhoneModeChange()
    {
        LOG_INFO("Phone mode changed to: [%s]. Notifying phone modes observers.",
                 magic_enum::enum_name(phoneMode).data());
        owner->bus.sendMulticast(std::make_shared<PhoneModeChanged>(phoneMode), BusChannel::PhoneModeChanges);
    }

    void Subject::notifyTetheringChange()
    {
        LOG_INFO("Tethering mode changed to: [%s]. Notifying phone modes observers.",
                 magic_enum::enum_name(tetheringMode).data());
        owner->bus.sendMulticast(std::make_shared<TetheringChanged>(tetheringMode), BusChannel::PhoneModeChanges);
    }
    bool Subject::isTetheringEnabled() const noexcept
    {
        return tetheringMode == Tethering::On;
    }

    bool Subject::isTetheringPossible() const noexcept
    {
        return phoneMode != PhoneMode::Offline;
    }
} // namespace sys::phone_modes
