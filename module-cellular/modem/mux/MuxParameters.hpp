// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

struct MuxParameters
{
    PortSpeed_e PortSpeed{}; //!< Port speed
    int MaxFrameSize{};      //!< Maximum Frame Size [ 1-128 in Basic mode, 1-512 in HDLC modes, default: 31 for basic
    //!< option & 64 for advanced ]
    int AckTimer{};                //!< Acknowledgement Timer [0,01s-2,55s, default: 0,1s]
    int MaxNumOfRetransmissions{}; //!< Maximum number of retransmissions [0-100, default: 3]
    int MaxCtrlRespTime{};         //!< Response timer for the multiplexer control channel [0,01s-2,55s, default: 0,3s]
    int WakeUpRespTime{};          //!< Wake up response timer [1s-255s, default: 10s]
    int ErrRecovWindowSize{};      //!< Window size for error recovery mode [1-7, default: 2]
};
