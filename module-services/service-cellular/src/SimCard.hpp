// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/response.hpp>
#include <at/SimState.hpp>
#include <service-cellular/api/common.hpp>

#include <module-cellular/at/SimInsertedState.hpp>

namespace at
{
    class Cmd;
    class BaseChannel;
} // namespace at

namespace cellular::service::sim
{
    enum class Pin
    {
        PIN1 = 0,
        PIN2 = 1
    };

    enum class Result
    {
        OK     = 0,
        Ready  = 1,
        Locked = 2, /*!< In case of attempt counters set to 0 */

        AT_ERROR_Begin = 9, /*!< this is only for separate AT SIM errors from new one added, AT errors list end with
                               AT_ERROR_End */

        SIMNotInserted    = 10,
        SIM_PIN_required  = 11,
        SIM_PUKRequired   = 12,
        Failure           = 13,
        Busy              = 14,
        Wrong             = 15,
        IncorrectPassword = 16,

        AT_ERROR_End = 17,

        Unknown = 0xFF /*!< Unknown, any reason (not only AT), in some case AT commends return just error, eg. twice
                                  supply good pin, second AT commend return ERROR */

    };

    enum class LockType
    {
        PIN,
        PUK
    };
} // namespace cellular::service::sim

namespace cellular::internal
{
    service::sim::Result convertErrorFromATResult(const at::Result atres);
    std::string simCodeToString(const cellular::api::SimCode &v);
} // namespace cellular::internal

namespace cellular::service
{
    class SimCard
    {
      public:
        /** Check if cmd channel is set
         * @return true if ready to communicate
         */
        bool ready() const;

        /** Check if card is fully initialized
         * @return true in card is fully operational
         */
        bool initialized() const;

        /** Set cmd channel
         * \param channel channel (or nullptr to block communication)
         */
        void setChannel(at::BaseChannel *channel);

        /**
         * Request message handlers
         */
        bool handleSetActiveSim(api::SimSlot sim);
        std::optional<bool> handleIsPinNeeded() const;
        bool handleChangePin(const api::SimCode &old_pin, const api::SimCode &pin);
        bool handleUnblockWithPuk(const api::SimCode &puk, const api::SimCode &pin);
        bool handleSetPinLock(const api::SimCode &pin, api::SimPinState pinLock);
        bool handlePinUnlock(const api::SimCode &pin);

        /**
         * Notification message handlers
         */
        void handleTrayState();

        /**
         * Internal message handlers
         */
        void handleATSimStateChange(at::SimState state);

        /**
         * Check if sim card is present in slot
         * @return true if sim card is present in slot
         */
        bool isSimCardInserted();

        /**
         * Set new sim inserted status
         * @param newStatus
         */
        void setSimInserted(at::SimInsertedStatus newStatus)
        {
            simInserted = newStatus;
        }

        /**
         * Gets sim inserted status
         * @return actual value of sim inserted status
         */
        std::optional<at::SimInsertedStatus> getSimInsertedStatus()
        {
            return simInserted;
        }

        /**
         * Clears sim inserted status
         */
        void clearSimInsertedStatus()
        {
            simInserted = std::nullopt;
        }

        /**
         * Gets Sim Select progress state
         * @return true if sim selecting is in progres
         */
        bool isSimSelectInProgress()
        {
            return simSelectInProgress;
        }

        /**
         * Sets Sim Select progress state
         * @param inProgress new progress state
         */
        void setSimSelectInProgress(bool inProgress)
        {
            simSelectInProgress = inProgress;
        }

        /**
         * Sim timer event handler
         */
        void handleSimTimer();

        /**
         * Sim Inserted notification handler
         * @param status new Sim Inserted status
         */
        void handleSimInsertionNotification(at::SimInsertedStatus status);

        /**
         * Handles post sim selection actions
         */
        void handleSimCardSelected();
        /**
         * Get Sim selected state
         * @return true when Sim is selected, fail when not
         */
        bool isSimCardSelected()
        {
            return isSimSelected;
        }

        /** Read internal SIM state using CPIN AT commands
         */
        std::optional<at::SimState> simState() const;

        /**
         * Notification events
         */
        std::function<void()> onSimReady;
        std::function<void(unsigned int attempts)> onNeedPin;
        std::function<void(unsigned int attempts)> onNeedPuk;
        std::function<void()> onSimBlocked;
        std::function<void()> onSimEvent;
        std::function<void(unsigned int code)> onUnhandledCME;
        std::function<void()> onSimNotPresent;
        std::function<void()> onSimSelected;

      private:
        /** SIM card initialization sequence
         * \return true on success, false on failure
         */
        bool initSimCard();

        /** Get information about attempts of PIN and PUK for standard sim card (optionally PIN2/PUK2)
         * \return  As optional SimCard::AttemptsCounters, in case of error nullopt. Should be noted that in some case
         * could return SIMFailure which could mean 0 attempts (happen if lock during session, on modem/sim reboot again
         * return 0,0);
         */
        std::optional<at::response::qpinc::AttemptsCounters> getAttemptsCounters(sim::Pin pin = sim::Pin::PIN1) const;

        /** Supply pin for modem
         * \param pin digits as a string from 4-8 digits
         * \return return OK on success in other case see details in SimCardResult
         */
        sim::Result supplyPin(const std::string &pin) const;

        /** Supply pin for modem
         * \param puk puk as standard 8 digits
         * \param pin, new pin digits as a string from 4-8 digits
         * \return return OK on success in other case see details in SimCardResult
         */
        sim::Result supplyPuk(const std::string &puk, const std::string &pin) const;

        /** Set whether to provide pin. Always need to provide actual pin for sim card, only for standard PIN
         * \param lock true for lock SIM card
         * \param pin actual pin for SIM card
         * \return
         */
        sim::Result setPinLock(const std::string &pin, bool lock) const;

        /** Change pin, only for standard pin. To get effect of change pin, SIM cart or modem should be restarted
         * simplest solution is to call AT+CFUN=0/1
         * \param oldPin
         * \param newPin
         * \return return OK on success, else see SimCardResult
         */
        sim::Result changePin(const std::string &oldPin, const std::string &newPin) const;

        /** Check whether the PIN needs to be provided, only for standard PIN.
         * \return true if need PIN to unlock SIM card functionality, false if don't need, std::nullopt if SIM card
         * not ready
         */
        std::optional<bool> isPinNeeded() const;

        /** Process sim::Result from PIN lock/unlock operations
         * \param result result from operation (`sendCommand()`)
         * \result return true on success
         */
        bool processPinResult(sim::Result result);

        sim::Result sendCommand(sim::LockType check, const at::Cmd &cmd) const;

        void handleSimState(at::SimState state);

        /**
         * Read sim card insert status
         * @return sim card inserted status
         */
        std::optional<at::SimInsertedStatus> readSimCardInsertStatus();

        at::BaseChannel *channel                         = nullptr;
        std::optional<api::SimSlot> sim                  = std::nullopt;
        std::optional<at::SimInsertedStatus> simInserted = std::nullopt;
        bool simSelectInProgress                         = false;
        bool isSimSelected                               = false;
    };

} // namespace cellular::service
