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

    void PinLock::putNextChar(unsigned int c) noexcept
    {
        if (state == State::EnterPin && charCount < pinValue.size()) {
            pinValue[charCount++] = c;
        }
    }

    void PinLock::verifyPin() noexcept
    {
        if (charCount == pinValue.size()) {
            handler->handle(pinValue);
            clearAttempt();
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

    bool PinLock::unlock() noexcept
    {
        if (state == State::VerifiedPin || pinValue.size() == 0) {
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

    void PinLock::reset(LockType newType, State newState, unsigned int attempts, unsigned int size) noexcept
    {
        type              = newType;
        state             = newState;
        remainingAttempts = attempts;
        pinValue          = std::vector<unsigned int>(size, 0);
    }
} // namespace gui
