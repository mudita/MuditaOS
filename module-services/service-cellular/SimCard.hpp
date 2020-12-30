// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <SimState.hpp>

#include "service-cellular/SimCardResult.hpp"
#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "response.hpp"

class SimCard
{
  public:
    explicit SimCard(ServiceCellular &cellularService) : cellularService(cellularService)
    {}

    /** Get information about attempts of PIN and PUK for standard sim card (eg. not PIN2)
     * @return  As optional SimCard::AttemptsCounters, in case of error nullopt. Should be noted that in some case could
     * return SIMFailure which could mean 0 attempts (happen if lock during session, on modem/sim reboot again return
     * 0,0);
     */
    std::optional<at::response::qpinc::AttemptsCounters> getAttemptsCounters() const;

    /** Supply pin for modem
     * \param pin digits as a string from 4-8 digits
     * \return return OK on success in other case see details in SimCardResult
     */
    SimCardResult supplyPin(const std::string pin) const;

    /** Supply pin for modem
     * \param puk puk as standard 8 digits
     * \param pin, new pin digits as a string from 4-8 digits
     * \return return OK on success in other case see details in SimCardResult
     */
    SimCardResult supplyPuk(const std::string puk, const std::string pin) const;

    /** return whether the pin needs to be provided, only for standard pin.
     * \return true if need pin to unlock SIM card functionality
     */
    bool isPinLocked() const;

    /** Set whether to provide pin. Always need to provide actual pin for sim card, only for standard PIN
     * \param lock true for lock SIM card
     * \param pin actual pin for SIM card
     * \return
     */
    SimCardResult setPinLock(bool lock, const std::string &pin) const;

    /** Change pin, only for standard pin. To get effect of change pin, SIM cart or modem should be restarted
     * simplest solution is to call AT+CFUN=0/1
     * \param oldPin
     * \param newPin
     * \return return OK on success, else see SimCardResult
     */
    SimCardResult changePin(const std::string oldPin, const std::string newPin) const;

    /** Return SIM state based on CPIN AT commands
     */
    std::optional<at::SimState> simState() const;
    std::optional<at::SimState> simStateWithMessage(std::string &message) const;

    static std::string pinToString(std::vector<unsigned int> v);

  private:
    ServiceCellular &cellularService;

    /** Helper function to convert from one enum to another (only with part of them). Base on continuous range of sim
     * errors in AT.
     */
    SimCardResult convertErrorFromATResult(const at::Result) const;
};
