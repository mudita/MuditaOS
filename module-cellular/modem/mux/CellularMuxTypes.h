// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef _TS0710_TYPES_H
#define _TS0710_TYPES_H

#include <map>
#include <string>
#include "bsp/cellular/bsp_cellular.hpp"

typedef int DLCI_t; //!< [1 – 63] DLC over which the data is to be transmitted.
enum TypeOfFrame_e
{
    SABM = 0x3F,
    UA   = 0x73,
    DM   = 0x1F,
    DISC = 0x53,
    UIH  = 0xEF,
    UI   = 0x03,
    I
};
extern std::map<TypeOfFrame_e, std::string> TypeOfFrame_text;
struct DLC_ESTABL_SystemParameters_t
{
    TypeOfFrame_e TypeOfFrame; //!< Type of frame [UIH | UI | I, default: UIH]
    int ConvergenceLayer;      //!< Convergence layer [1 - 4, default: 1]
    int Priority;              //!< Priority [0-63]
    int AckTime;               //!< Acknowledgement Timer [0,01s-2,55s, default: 0,1s]
    int MaxFrameSize; //!< Maximum Frame Size [1 – 32768, default: 31 for the basic option and 64 for the advanced
                      //!< option]
    int MaxNumOfRetransmissions; //!< Maximum number of retransmissions [0 – 255, default : 3]
    int ErrRecovWindowSize;      //!< Window size for error recovery mode [1 – 7, default : 2]
};

enum PortSpeed_e
{
    PS2400,
    PS4800,
    PS7200,
    PS9600,
    PS19200,
    PS38400,
    PS57600,
    PS115200,
    PS230400,
    PS460800
};
extern std::map<PortSpeed_e, int> QuectelCMUXPortSpeeds_text;
extern std::map<PortSpeed_e, int> ATPortSpeeds_text;

/*MUX defines */
enum class MuxDefines
{
    GSM0710_FRAME_FLAG          = 0xF9, // basic mode flag for frame start and end
    GSM0710_FRAME_ADV_FLAG      = 0x7E, // advanced mode flag for frame start and end
    GSM0710_FRAME_ADV_ESC       = 0x7D, // advanced mode escape symbol
    GSM0710_FRAME_ADV_ESC_COPML = 0x20, // advanced mode escape complement mask
                                        // bits: Poll/final, Command/Response, Extension
    GSM0710_PF = 0x10,                  // 16
    GSM0710_CR = 0x02,                  // 2
    GSM0710_EA = 0x01,                  // 1
    // type of frames
    GSM0710_TYPE_SABM = 0x2F, // 47 Set Asynchronous Balanced Mode
    GSM0710_TYPE_UA   = 0x63, // 99 Unnumbered Acknowledgement
    GSM0710_TYPE_DM   = 0x0F, // 15 Disconnected Mode
    GSM0710_TYPE_DISC = 0x43, // 67 Disconnect
    GSM0710_TYPE_UIH  = 0xEF, // 239 Unnumbered information with header check
    GSM0710_TYPE_UI   = 0x03, // 3 Unnumbered Acknowledgement
    // control channel commands
    GSM0710_CONTROL_PN   = (0x80 | GSM0710_EA), //?? DLC parameter negotiation
    GSM0710_CONTROL_CLD  = (0xC0 | GSM0710_EA), // 193 Multiplexer close down
    GSM0710_CONTROL_PSC  = (0x40 | GSM0710_EA), //??? Power Saving Control
    GSM0710_CONTROL_TEST = (0x20 | GSM0710_EA), // 33 Test Command
    GSM0710_CONTROL_MSC  = (0xE0 | GSM0710_EA), // 225 Modem Status Command
    GSM0710_CONTROL_NSC  = (0x10 | GSM0710_EA), // 17 Non Supported Command Response
    GSM0710_CONTROL_RPN  = (0x90 | GSM0710_EA), //?? Remote Port Negotiation Command
    GSM0710_CONTROL_RLS  = (0x50 | GSM0710_EA), //?? Remote Line Status Command
    GSM0710_CONTROL_SNC  = (0xD0 | GSM0710_EA), //?? Service Negotiation Command
                                                // V.24 signals: flow control, ready to communicate, ring indicator,
                                                // data valid
    GSM0710_SIGNAL_FC  = 0x02,
    GSM0710_SIGNAL_RTC = 0x04,
    GSM0710_SIGNAL_RTR = 0x08,
    GSM0710_SIGNAL_IC  = 0x40, // 64
    GSM0710_SIGNAL_DV  = 0x80, // 128
    GSM0710_SIGNAL_DTR = 0x04,
    GSM0710_SIGNAL_DSR = 0x04,
    GSM0710_SIGNAL_RTS = 0x08,
    GSM0710_SIGNAL_CTS = 0x08,
    GSM0710_SIGNAL_DCD = 0x80 // 128
};

#endif /*_TS0710_TYPES_H*/
