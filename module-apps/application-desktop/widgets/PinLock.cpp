// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PinLock.hpp"
#include "PinLockHandler.hpp"

namespace gui
{
    PinLock::PinLock(gui::PinLockHandler *handler) : handler(handler)
    {}

    void PinLock::consumeInvalidPinState() noexcept
    {
        if (state == State::InvalidPin) {
            state = State::EnterPin;
        }
    }

    void PinLock::putNextChar(unsigned int c)
    {
        if (state == State::EnterPin && maxPinSize > pinValue.size()) {
            pinValue.push_back(c);
        }
    }

    void PinLock::verifyPin()
    {
        handler->handle(pinValue);
        clearAttempt();
    }

    void PinLock::popChar()
    {
        if (state == State::EnterPin && pinValue.size() > 0) {
            pinValue.pop_back();
        }
    }

    void PinLock::clearAttempt() noexcept
    {
        pinValue.clear();
    }

    bool PinLock::unlock() noexcept
    {
        if (state == State::VerifiedPin || maxPinSize == 0) {
            state = State::Unlocked;
            return true;
        }
        return false;
    }

    void PinLock::lock() noexcept
    {
        if (state == State::Unlocked) {
            state = State::EnterPin;
        }
    }

    bool PinLock::isLocked() const noexcept
    {
        return state != State::Unlocked;
    }

    std::string PinLock::getLockInfo(const InfoName name) const
    {
        try {
            return additionalLockInfo.at(name);
        }
        catch (std::out_of_range &) {
            return std::string{};
        }
    }

    void PinLock::reset(LockType _type,
                        State _state,
                        unsigned int _remainingAttempts,
                        unsigned int _maxPinSize,
                        unsigned int _minPinSize) noexcept
    {
        type              = _type;
        state             = _state;
        remainingAttempts = _remainingAttempts;
        pinValue          = std::vector<unsigned int>();
        maxPinSize        = _maxPinSize;
        minPinSize        = _minPinSize;
    }
} // namespace gui
