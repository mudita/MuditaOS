// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <SimState.hpp>

#include "SimCardResult.hpp"
#include "CellularMessage.hpp"
#include "CellularServiceAPI.hpp"
#include "ServiceCellular.hpp"

namespace simcard
{
    enum class SimCardResult1
    {
        OK     = 0,
        Ready  = 1,
        Locked = 2, /*!< In case of attempt counters set to 0 */

        AT_ERROR_Begin =
            9, /*!< this is only for separate AT SIM errors from new one added, AT errors list end with AT_ERROR_End */

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
}

class SimCard
{
  public:
    SimCard(ServiceCellular &cellularService) : cellularService(cellularService)
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
