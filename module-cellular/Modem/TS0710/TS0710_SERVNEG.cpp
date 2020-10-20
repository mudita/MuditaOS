// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/**
 * Project Untitled
 */

#include "TS0710_SERVNEG.h"

/**
 * TS0710_SERVNEG implementation
 */

/**
 * @param DLC
 * @param Service_parameters
 */
void TS0710_SERVNEG::request(DLCI_t DLC, ServiceParameters_t Service_parameters)
{}

/**
 * @param DLC
 * @param Service_parameters
 */
void TS0710_SERVNEG::indication(DLCI_t DLC, ServiceParameters_t Service_parameters)
{}

/**
 * @param DLC
 * @param Service_parameters
 * @param accept
 */
void TS0710_SERVNEG::response(DLCI_t DLC, ServiceParameters_t Service_parameters, bool accept)
{}

/**
 * @param DLC
 * @param Service_parameters
 * @param accept
 */
void TS0710_SERVNEG::confirm(DLCI_t DLC, ServiceParameters_t Service_parameters, bool accept)
{}