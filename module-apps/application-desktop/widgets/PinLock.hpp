#pragma once

#include "PhoneNumber.hpp"
namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class PinLock
    {
      public:
        enum class LockType
        {
            Screen,
            SIM,
            PUK,
            Unknown
        };
        enum class InfoName
        {
            LockName,
            PhoneNum
        };
        enum class State
        {
            EnterPin,
            InvalidPin,
            VerifiedPin,
            Blocked,
            Unlocked
        };

        State getState() const noexcept
        {
            return state;
        }
        unsigned int getPinSize() const noexcept
        {
            return pinValue.size();
        }
        /// returns current position of a PIN character to be inserted
        unsigned int getCharCount() const noexcept
        {
            return charCount;
        }
        unsigned int getRemainingAttempts() const noexcept
        {
            return remainingAttempts;
        }
        void putNextChar(unsigned int c) noexcept;
        void verifyPin() noexcept;
        /// removes a last character passed to Lock via putNextChar. The last character can not be popped
        void popChar() noexcept;
        /// clear all characters passed to the Lock
        void clearAttempt() noexcept;
        /// if Lock is in the State::InvalidPin state, changes it's state to the State::EnterPin
        void consumeInvalidPinState() noexcept;

        bool isLocked() const noexcept;
        bool unlock() noexcept;
        void lock() noexcept;

        std::string getLockInfo(const InfoName name) const;
        LockType getLockType() const noexcept
        {
            return type;
        }
        PinLock(app::ApplicationDesktop *);

      private:
        /// for PIN verification purposes as PIN storage and management is out of scope of PinLock class
        app::ApplicationDesktop *appDesktop;

        LockType type                  = LockType::Unknown;
        State state                    = State::EnterPin;
        unsigned int remainingAttempts = 0;
        /// code of the entered character on specified position
        std::vector<unsigned int> pinValue;
        /// flag defines number of entered pin characters
        unsigned int charCount = 0;
        std::map<InfoName, std::string> additionalLockInfo;

        void reset(LockType, State, unsigned int remainingAttempts, unsigned int pinSize) noexcept;

        friend class app::ApplicationDesktop;
    };

} // namespace gui
