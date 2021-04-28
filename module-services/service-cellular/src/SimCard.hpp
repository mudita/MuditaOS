// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/response.hpp>
#include <at/SimState.hpp>
#include <service-cellular/api/common.hpp>

namespace at
{
    class Cmd;
    class Channel;
}
class ServiceCellular;

namespace cellular::service
{
    namespace sim
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
    } // namespace sim

    class SimCard
    {
      public:
        SimCard()
        {}

        /** Connect request::sim Messages directly to their handlers
         */
        void registerMessages(ServiceCellular *owner);

        /** Check if sim card slot has been selected and cmd channel is set
         * @return true if ready to communicate
         */
        bool ready() const;

        /** Select active SIM slot
         * \param sim slot to communicate with
         */
        void selectSim(api::Sim slot);

        /** Set cmd channel
         * \param cmd channel (or nullptr to reset)
         */
        void resetChannel(at::Channel *cmd = nullptr);

        /** Get information about attempts of PIN and PUK for standard sim card (optionally PIN2/PUK2)
         * @return  As optional SimCard::AttemptsCounters, in case of error nullopt. Should be noted that in some case
         * could return SIMFailure which could mean 0 attempts (happen if lock during session, on modem/sim reboot again
         * return 0,0);
         */
        std::optional<at::response::qpinc::AttemptsCounters> getAttemptsCounters(sim::Pin pin = sim::Pin::PIN1) const;

        /** Supply pin for modem
         * \param pin digits as a string from 4-8 digits
         * \return return OK on success in other case see details in SimCardResult
         */
        sim::Result supplyPin(const std::string pin) const;

        /** Supply pin for modem
         * \param puk puk as standard 8 digits
         * \param pin, new pin digits as a string from 4-8 digits
         * \return return OK on success in other case see details in SimCardResult
         */
        sim::Result supplyPuk(const std::string puk, const std::string pin) const;

        /** Set whether to provide pin. Always need to provide actual pin for sim card, only for standard PIN
         * \param lock true for lock SIM card
         * \param pin actual pin for SIM card
         * \return
         */
        sim::Result setPinLock(bool lock, const std::string &pin) const;

        /** Change pin, only for standard pin. To get effect of change pin, SIM cart or modem should be restarted
         * simplest solution is to call AT+CFUN=0/1
         * \param oldPin
         * \param newPin
         * \return return OK on success, else see SimCardResult
         */
        sim::Result changePin(const std::string oldPin, const std::string newPin) const;

        /** Return SIM state based on CPIN AT commands
         */
        std::optional<at::SimState> simState() const;

      private:
        at::Channel *channel        = nullptr;
        std::optional<api::Sim> sim = std::nullopt;
    };

} // namespace cellular::service
