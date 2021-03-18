// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Subject.hpp"

#include <Service/Service.hpp>

#include <module-utils/magic_enum/include/magic_enum.hpp>

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
        const bool changed = changePhoneMode(_phoneMode) || changeTetheringMode(_tetheringMode);
        if (changed) {
            notifyChange();
        }
        return changed;
    }

    bool Subject::setPhoneMode(PhoneMode mode)
    {
        const auto changed = changePhoneMode(mode);
        if (changed) {
            notifyChange();
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
            notifyChange();
        }
        return changed;
    }

    bool Subject::changeTetheringMode(Tethering mode) noexcept
    {
        return std::exchange(tetheringMode, mode) != mode;
    }

    void Subject::notifyChange()
    {
        LOG_INFO("Phone modes changed: Phone mode: [%s]; Tethering: [%s]. Notifying phone modes observers.",
                 magic_enum::enum_name(phoneMode).data(),
                 magic_enum::enum_name(tetheringMode).data());
        auto message = std::make_shared<PhoneModeChanged>(phoneMode, tetheringMode);
        owner->bus.sendMulticast(std::move(message), BusChannel::PhoneModeChanges);
    }
} // namespace sys::phone_modes
