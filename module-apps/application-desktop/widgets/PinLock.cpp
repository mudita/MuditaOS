#include "PinLock.hpp"
namespace gui
{

    PinLock::PinLock(app::Application *app, const LockType lockType, State defState) : application(app), type(lockType)
    {
        info.infos[LockInfo::InfoName::LockName] = toString(type);
        info.pinSize                             = 0;
        reset(defState);
    }

    void PinLock::consumeInvalidPinState() noexcept
    {
        if (state == State::InvalidPin) {
            state = State::EnterPin;
        }
    }
    void PinLock::putNextChar(unsigned int c) noexcept
    {
        if (state == State::EnterPin || state == State::InvalidPin) {
            if (charCount == 0) {
                state = State::EnterPin;
            }
            pinValue[charCount++] = c;
            if (charCount == pinValue.size()) {
                updateState();
            }
        }
    }

    void PinLock::popChar() noexcept
    {
        if (state == State::EnterPin && charCount > 0) {
            charCount--;
            pinValue[charCount] = 0;
        }
    }

    void PinLock::clearAttempt() noexcept
    {
        for (auto &c : pinValue) {
            c = 0;
        }
        charCount = 0;
    }

    void PinLock::updateState() noexcept
    {
        if (isPinValid()) {
            state = State::VerifiedPin;
        }
        else {
            remainingAttempts--;
            if (remainingAttempts > 0) {
                state = State::InvalidPin;
            }
            else {
                state = State::PhoneBlocked;
            }
        }
        clearAttempt();
    }

    bool PinLock::isPinValid()
    {
        if (application->getSettings().lockPassHash == hashPin()) {
            remainingAttempts = maxPasswordAttempts;
            return true;
        }
        return false;
    }

    uint32_t PinLock::hashPin() const noexcept
    {
        uint32_t value = 0;
        for (auto c : pinValue) {
            value = 10 * value + c;
        }
        return value;
    }

    void PinLock::reset(State current, State defaultWhenLocked)
    {
        if (info.pinSize == 0) {
            defaultState = State::VerifiedPin;
            state        = defaultState;
        }
        else {
            defaultState = defaultWhenLocked;
            state        = current;
        }
        pinValue.resize(info.pinSize);
        clearAttempt();
        remainingAttempts = maxPasswordAttempts;
    }

    void PinLock::setLockInfo(LockInfo::InfoName name, const std::string &value)
    {
        info.infos[name] = value;
    }

    void PinLock::lock() noexcept
    {
        if (state == State::Unlocked) {
            clearAttempt();
            remainingAttempts = maxPasswordAttempts;
            state             = defaultState;
        }
    }

    bool PinLock::isLocked() const noexcept
    {
        return state != State::Unlocked;
    }

    void PinLock::unlock() noexcept
    {
        if (state == State::VerifiedPin) {
            state = State::Unlocked;
        }
    }
    std::string PinLock::toString(const PinLock::LockType t)
    {
        switch (t) {
        case LockType::Screen:
            return std::string{"Screen"};
        case LockType::PUK:
            return std::string{"PUK"};
        case LockType::SIM1:
            return std::string{"SIM1"};
        case LockType::SIM2:
            return std::string{"SIM2"};
        };
        return std::string{};
    }

    std::string PinLock::getLockInfo(const LockInfo::InfoName name) const
    {
        try {
            return info.infos.at(name);
        }
        catch (std::out_of_range &) {
            return std::string{};
        }
    }

} // namespace gui
