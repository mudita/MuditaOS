// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace at
{
    /// SimState extends AT states based on CPIN (URC and AT Command)
    enum class SimState
    {
        Ready,
        NotReady,

        SimPin,
        SimPuk,
        SimPin2,
        SimPuk2,
        PhNetPin,
        PhNetPuk,
        PhNetSPin,
        PhNetSPuk,
        PhSpPin,
        PhSpPuk,
        PhCorpPin,
        PhCorpPuk,

        Locked, ///< In case of attempt counters equal zero
        Unknown
    };
} // namespace at
