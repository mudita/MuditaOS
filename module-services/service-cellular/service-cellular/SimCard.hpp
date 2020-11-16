// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <SimState.hpp>

#include "SimCardResult.hpp"
#include "CellularMessage.hpp"
#include "CellularServiceAPI.hpp"
#include "ServiceCellular.hpp"

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
    std::optional<SimCard::AttemptsCounters> getAttemptsCounters();

    SimCardResult supplyPin(const std::string pin);
    SimCardResult supplyPuk(const std::string puk, const std::string pin);
    bool isPinLocked();
    SimCardResult setPinLock(bool lock, std::string pin);
    SimCardResult changePin(std::string oldPin, std::string newPin);

    at::SimState simState();
    at::SimState simStateWithMessage(std::string &message);

  private:
    ServiceCellular &cellularService;

    SimCardResult convertErrorFromATResult(at::Result);
};
