// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <SimState.hpp>

#include "service-cellular/SimCardResult.hpp"
#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"

class SimCard
{
  public:
    explicit SimCard(ServiceCellular &cellularService) : cellularService(cellularService)
    {}

    /// Structure that holds parsed information from AT+QPINC command
    struct AttemptsCounters
    {
        int PinCounter; /*!<  PIN attempts counter */
        int PukCounter; /*!<  PUK attempts counter */
    };

    /** Get information about attempts of PIN and PUK for standard sim card (eg. not PIN2)
     * @return  As optional SimCard::AttemptsCounters, in case of error nullopt. Should be noted that in some case could
     * return SIMFailure which could mean 0 attempts (happen if lock during session, on modem/sim reboot again return 0,0);
     */
    std::optional<SimCard::AttemptsCounters> getAttemptsCounters();

    /** Supply pin for modem
     * \param pin digits as a string from 4-8 digits
     * \return return OK on success in other case see details in SimCardResult
     */
    SimCardResult supplyPin(const std::string pin);

    /** Supply pin for modem
     * \param puk puk as standard 8 digits
     * \param pin, new pin digits as a string from 4-8 digits
     * \return return OK on success in other case see details in SimCardResult
     */
    SimCardResult supplyPuk(const std::string puk, const std::string pin);

    /** return whether the pin needs to be provided, only for standard pin.
     * \return true if need pin to unlock SIM card functionality
     */
    bool isPinLocked();

    /** Set whether to provide pin. Always need to provide actual pin for sim card, only for standard PIN
     * \param lock true for lock SIM card
     * \param pin actual pin for SIM card
     * \return
     */
    SimCardResult setPinLock(bool lock, std::string pin);

    /** Change pin, only for standard pin. To get effect of change pin, SIM cart or modem should be restarted
     * simplest solution is to call AT+CFUN=0/1
     * \param oldPin
     * \param newPin
     * \return return OK on success, else see SimCardResult
     */
    SimCardResult changePin(std::string oldPin, std::string newPin);

    /** Return SIM state based on CPIN AT commands
     */
    std::optional<at::SimState> simState();
    std::optional<at::SimState> simStateWithMessage(std::string &message);

  private:
    ServiceCellular &cellularService;

    /** Helper function to convert from one enum to another (only with part of them). Base on continuous range of sim errors
    * in AT.
    */
    SimCardResult convertErrorFromATResult(at::Result);

    std::optional<std::vector<std::string>> getTokensForATCommand( const at::Result &resp , std::string_view head );
};
