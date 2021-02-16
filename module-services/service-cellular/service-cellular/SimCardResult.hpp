// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
class SimPinType
{
  public:
    static constexpr auto SimPin  = "SC";
    static constexpr auto SimPin2 = "P2";
};
enum class SimCardResult
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
