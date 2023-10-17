// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/**
 * Project Untitled
 * @brief The transmitting unit initiates transmission of data using the frame type specified for the chosen DLCI by
 * means of the request primitive. The transmitted data is delivered to the upper layer of the receiving by the
 * indication primitive. No confirmation primitive exists even for the error recovery mode. In this mode TS 0710 will
 * take care of all mechanisms involved in the error checking and thus deliver data error free.
 */

#pragma once

#include <vector>
#include "CellularMux.h"

class CellularMuxData
{
  public:
    CellularMuxData(DLCI_t DLCI,
                    DLC_ESTABL_SystemParameters_t sysParams,
                    const std::vector<uint8_t> &userData,
                    bsp::Cellular *cellular);

  private:
    /**
     * @param DLCI
     * @param userData
     */
    void request(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t sysParams, const std::vector<uint8_t> &userData);

    /**
     * @param DLCI
     * @param User_data
     *
     */
    void indication(DLCI_t DLCI, std::vector<uint8_t> User_data);

    bsp::Cellular *pvCellular;
};
