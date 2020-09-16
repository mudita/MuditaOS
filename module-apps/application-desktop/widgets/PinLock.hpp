#pragma once

#include "Application.hpp"

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{

    struct LockInfo
    {
        enum class InfoName
        {
            LockName,
            PhoneNum
        };

        unsigned int pinSize = 0;
        std::map<InfoName, std::string> infos;
    };

    class PinLock
    {
      public:
        enum class State
        {
            EnterPin,
            InvalidPin,
            VerifiedPin,
            PhoneBlocked,
            Unlocked
        };

        enum class LockType
        {
            Screen,
            PUK,
            SIM1,
            SIM2
        };

        State getState() const noexcept
        {
            return state;
        }
        uint32_t getPinSize() const noexcept
        {
            return info.pinSize;
        }
        /// returns current position of a PIN character to be inserted
        uint32_t getCharCount() const noexcept
        {
            return charCount;
        }
        uint32_t getRemainingAttempts() const noexcept
        {
            return remainingAttempts;
        }
        /// puts next character of a PIN. If PIN passed by user reaches a expected PIN size, function automatically
        /// proceeds with a verification process and updates the state of the Lock
        void putNextChar(unsigned int c) noexcept;
        /// removes a last character passed to Lock via putNextChar. The last character can not be popped
        void popChar() noexcept;
        /// clear all characters passed to the Lock
        void clearAttempt() noexcept;
        /// if Lock is in the State::InvalidPin state, changes it's state to the State::EnterPin
        void consumeInvalidPinState() noexcept;

        /// if Lock is in the State::VerifiedPin state, unlocks the Lock setting it into State::Unlocked
        void unlock() noexcept;
        /// if Lock is in the State::Unlocked state, locks the Lock setting it into defaultState
        void lock() noexcept;
        bool isLocked() const noexcept;

        std::string getLockInfo(const LockInfo::InfoName name) const;
        LockType getLockType() const
        {
            return type;
        }

      private:
        PinLock(app::Application *app, const LockType lockType, State defaultState = State::EnterPin);
        /// for PIN verification purposes as PIN storage and management is out of scope of PinLock class
        app::Application *application = nullptr;

        LockType type;
        LockInfo info;
        State defaultState;
        State state;
        const unsigned int maxPasswordAttempts = 4;
        uint32_t remainingAttempts             = maxPasswordAttempts;
        /// code of the entered character on specified position
        std::vector<unsigned int> pinValue;
        /// flag defines number of entered characters
        unsigned int charCount = 0;

        void reset(State current, State defaultWhenLocked = State::EnterPin);
        void setLockInfo(LockInfo::InfoName name, const std::string &value);
        bool isPinValid();
        void updateState() noexcept;
        uint32_t hashPin() const noexcept;
        static std::string toString(const LockType type);
        friend class app::ApplicationDesktop;
    };

} // namespace gui
