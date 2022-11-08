// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/widgets/Lock.hpp>
#include <locks/data/SimLockMessages.hpp>
#include <locks/data/LockData.hpp>

#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>

namespace locks
{
    using StoredLockInput = std::vector<unsigned int>;

    class SimLockHandler
    {
      private:
        sys::Service *owner;
        Lock lock;

        SimInputTypeAction simInputTypeAction = SimInputTypeAction::UnlockWithPin;
        unsigned int storedErrorCode          = 0;
        bool simUnlockingNeeded_              = false;
        bool simReady                         = false;
        StoredLockInput storedFirstInput;
        StoredLockInput storedSecondInput;

        void clearStoredInputs();
        void setSimInputTypeAction(SimInputTypeAction _simInputTypeAction);

        void simInputRequiredAction();
        void simErrorAction(unsigned int errorCode);
        void simUnlockAction();
        void simInfoAction();
        void simNotReadyAction();
        void simReadyAction();

        sys::MessagePointer unlockSimWithPin(LockInput pinInputData);
        sys::MessagePointer processLockWithNewInput(LockInput inputData);
        sys::MessagePointer resolveNewInputAction(LockInput firstInputData, LockInput secondInputData);
        sys::MessagePointer unlockSimWithPuk(LockInput pukInputData, LockInput newPinInputData);
        sys::MessagePointer changeSimPin(LockInput oldPinInputData, LockInput newPinInputData);
        sys::MessagePointer enableSimPin(LockInput pinInputData);
        sys::MessagePointer disableSimPin(LockInput pinInputData);

      public:
        explicit SimLockHandler(sys::Service *owner);

        void setSimReady();

        void needSimUnlocking();
        sys::MessagePointer askForSimUnlocking();

        sys::MessagePointer verifySimLockInput(LockInput inputData);

        sys::MessagePointer handleSimPinRequest(unsigned int attempts);
        sys::MessagePointer handleSimPukRequest(unsigned int attempts);
        sys::MessagePointer handleSimPinChangeRequest();
        sys::MessagePointer handleSimPinChangeFailedRequest();
        sys::MessagePointer handleSimPinLockEnableRequest();
        sys::MessagePointer handleSimPinLockDisableRequest();
        sys::MessagePointer handleResetSimLockStateRequest();
        sys::MessagePointer handleSimBlockedRequest();
        sys::MessagePointer handleCMEErrorRequest(unsigned int errorCode);
        sys::MessagePointer handleSimUnlockedMessage();
        sys::MessagePointer handleSimPinChangedMessage();
        sys::MessagePointer handleSimPinLockStateMessage();
        sys::MessagePointer handleSimReadyMessage();
        sys::MessagePointer handleSimNotInsertedMessage();
        sys::MessagePointer handleSimNotRespondingMessage();
        sys::MessagePointer handleSimUnlockRequest();

        void getSettingsSimSelect(const std::string &settingsSim);
        void setSim(cellular::api::SimSlot simSlot);
    };

} // namespace locks
