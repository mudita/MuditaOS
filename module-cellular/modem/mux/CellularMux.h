// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

/*
5.2.6 Basic Option
In this case, opening flag and closing flags may appear in the Information field of the frame. The flags cannot be used
to determine beginning and end of a frame. A length indication for the frame must be given instead. The frame structure
is then as follows:
-----------------------------------------------------------------------------------------
|  Flag   |  Adress | Control | Length indicator | Information      |   FCS   |  Flag   |
| 1 octet | 1 octet | 1 octet |  1 or 2 octets   | integral num. of | 1 octet | 1 octet |
|         |         |         |                  | octets           |         |         |
-----------------------------------------------------------------------------------------

NOTE:
    MSB           LSB
bit 8 7 6 5 4 3 2 1

Flag octet:
----------------------------------------
| bit  | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|      | 1 | 0 | 0 | 1 | 1 | 1 | 1 | 1 |
----------------------------------------

Address octet:
------------------------------------------
| bit | 1  | 2   | 3 | 4 | 5 | 6 | 7 | 8 |
|     | EA | C/R | D L C I               |
------------------------------------------
eg. 0x27 -> 00100111 (DLCI=9, C/R=1, EA=1)

C/R (Command/response bit):
---------------------------------------------------------
| Command/response | Direction              | C/R value |
| Command          | Initiator -> Responder |     1     |
|                  | Responder -> Initiator |     0     |
| Response         | Initiator -> Responder |     0     |
|                  | Responder -> Initiator |     1     |
---------------------------------------------------------

Control field (Table 2):
------------------------------------------------
| Frame Type | 1 | 2 | 3 | 4 |  5  | 6 | 7 | 8 |
| SABM       | 1 | 1 | 1 | 1 | P/F | 1 | 0 | 0 |
| UA         | 1 | 1 | 0 | 0 | P/F | 1 | 1 | 0 |
| DM         | 1 | 1 | 1 | 1 | P/F | 0 | 0 | 0 |
| DISC       | 1 | 1 | 0 | 0 | P/F | 0 | 1 | 0 |
| UIH        | 1 | 1 | 1 | 1 | P/F | 1 | 1 | 1 |
| UI         | 1 | 1 | 0 | 0 | P/F | 0 | 0 | 0 |
------------------------------------------------
P/F is the Poll/Final bit
SABM (Set Asynchronous Balanced Mode)
UA (Unnumbered Acknowledgement)
DM (Disconnected Mode)
DISC (Disconnect)
UIH (Unnumbered Information with Header check)
UI (Unnumbered Information)

The poll (P) bit set to 1 shall be used by a station to solicit (poll) a response or sequence of responses from the
other station. The final (F) bit set to 1 shall be used by a station to indicate the response frame transmitted as the
result of a soliciting (poll) command. The poll/final (P/F) bit shall serve a function in both command frames and
response frames. (In command frames, the P/F bit is referred to as the P bit. In response frames, it is referred to as
the F bit.)

Length indicator:
first byte (and only one if EA=1)
-------------------------------------------------
| bit  |  1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  |
|      | E/A | L1 | L2 | L3 | L4 | L5 | L6 | L7 |
-------------------------------------------------
second byte (if EA=0)
------------------------------------------------------
| bit  | 1  | 2  | 3   | 4   | 5   | 6   | 7   | 8   |
|      | L8 | L9 | L10 | L11 | L12 | L13 | L14 | L15 |
------------------------------------------------------

Frame Checking Sequence Field (FCS):
The FCS shall be the ones complement of the sum (modulo 2) of
    a) the remainder of
        x^k (x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x^1 + 1)
        divided (modulo 2) by the generator polynomial
        x^8 + x^2 + x + 1,
        where k is the number of bits in the frame existing between, but not including, the final bit of the opening
flag and the first bit of the FCS, excluding start and stop elements (start/stop transmission), and bits (synchronous
transmission) and octets (start/stop transmission) inserted for transparency, and b) the remainder of the division
(modulo 2) by the generator polynomial x^8 + x^2 + x + 1 of the product of x 8 by the content of the frame existing
between, but not including, the final bit of the opening flag and the first bit of the FCS, excluding start and stop
elements (start/stop transmission), and bits (synchronous transmission) and octets (start/stop transmission) inserted
for transparency. As a typical implementation, at the transmitter, the initial content of the register of the device
computing the remainder of the division is preset to all ones and is then modified by division by the generator
polynomial (as described above) of the address, control and information fields; the ones complement of the resulting
remainder is transmitted as the 8-bit FCS. At the receiver, the initial content of the register of the device computing
the remainder is preset to all ones. The final remainder after multiplication by x 8 and then division (modulo 2) by the
generator polynomial x^8 + x^2 + x + 1 of the serial incoming protected bits and the FCS, will be 1111 0011 (x 7 through
x 0 , respectively) in the absence of transmission errors. In the case of the UIH frame the FCS is calculated on the
contents of the address and control fields only. This means that the I-field is not protected but does permit
pre-calculation of the FCS for the repertoire of DLCIs that are to be used. The FCS is calculated in the normal manner
for all other frames in Table 2.

All transmitted characters will be sent using one start bit, eight data bits, no parity bit and one stop bit.
In the field descriptions, bit 1 is transmitted first.
Addresses, commands, responses and sequence numbers shall be transmitted low-order bit first (for example, the first bit
of the sequence number that is transmitted shall have the weight 2^0 ).
The FCS shall be transmitted to the line commencing with the coefficient of the highest term.
    NOTE: The use of these conventions in this Specification means that octet values are often expressed in the
            reverse bit order from conventions used in many other standards. The conventions are used here because
            of the importance of the correct order of bit transmission; care should be taken during implementation.

5.4.2 DLC Release
The release of a DLC may be initiated by either station by the transmission of a DISC frame with the P-bit set to one.
Confirmation of the DLC release is signalled by the other station sending a UA frame with the F-bit set to 1. Once the
DLC has been released the stations enter disconnected mode for that particular DLC.
If the station receiving the DISC command is already in a disconnected mode it will send a DM response.
If no UA or DM response has been received after T1 the initiating station may retransmit the DISC. This action may be
repeated until a response is obtained or action is taken by a higher layer.

5.3.1 Set Asynchronous Balanced Mode (SABM) command
The SABM command shall be used to place the addressed station in the Asynchronous Balanced Mode (ABM) where
all control fields shall be one octet in length. The station shall confirm acceptance of the SABM command by
transmission of a UA response at the first opportunity. Upon acceptance of this command, the DLC send and receive
state variables shall be set to zero.

5.3.2 Unnumbered Acknowledgement (UA) response
The UA response shall be used by the station to acknowledge the receipt and acceptance of SABM and DISC
commands.

5.3.3 Disconnected Mode (DM) response
The DM response shall be used to report a status where the station is logically disconnected from the data link. When in
disconnected mode no commands are accepted until the disconnected mode is terminated by the receipt of a SABM
command. If a DISC command is received while in disconnected mode a DM response should be sent.

5.3.4 Disconnect (DISC) command
The DISC command shall be used to terminate an operational or initialization mode previously set by a command. It
shall be used to inform one station that the other station is suspending operation and that the station should assume a
logically disconnected mode. Prior to actioning the command, the receiving station shall confirm the acceptance of the
DISC command by the transmission of a UA response.
DISC command sent at DLCI 0 have the same meaning as the Multiplexer Close Down command (see subclause
5.4.6.3.3). See also subclause 5.8.2 for more information about the Close-down procedure.

5.3.5 Unnumbered information with header check (UIH) command and
response
The UIH command/response shall be used to send information without affecting the V(S) or V(R) variables at either
station. UIH is used where the integrity of the information being transferred is of lesser importance than its delivery
to the correct DLCI. For the UIH frame, the FCS shall be calculated over only the address and control fields. Reception
of the UIH command/response is not sequence number verified by the data link procedures; therefore, the UIH frame may be
lost if a data link exception occurs during transmission of the protected portion of the command, or duplicated if an
exception condition occurs during any reply to the command. There is no specified response to the UIH command/response.

5.3.6 Unnumbered Information (UI) command and response
The UI command/response shall be used to send information without affecting the V(S) or V(R) variables at either
station.. Reception of the UIH command/response is not sequence number verified by the data link procedures;
therefore, the UIH frame may be lost if a data link exception occurs during transmission of the protected portion of the
command, or duplicated if an exception condition occurs during any reply to the command. There is no specified
response to the UI command/response.
For the UI frame, the FCS shall be calculated over all fields (Address, Control, Length Indicator, Information).
Support of UI frames is optional.

OPERATION:

5.4.1 DLC Establishment
In most cases the establishment of a DLC will be initiated by the TE, however, the protocol is balanced and the
initiation may come from the MS. The action taken by the higher layers of the TE upon the initiation of the
establishment of a DLC from the MS is outside the scope of this specification. The station wishing to establish a DLC
transmits a SABM frame with the P-bit set to 1. The address field contains the DLCI value associated with the desired
connection. If the responding station is ready to establish the connection it will reply with a UA frame with the F-bit
set to 1. If the responding station is not ready or unwilling to establish the particular DLC it will reply with a DM
frame with the F-bit set to 1. Once a DLC has been established the stations are both said to be in a connected mode, for
the particular DLC, and transfer of information may commence. If no UA or DM response has been received after T1 the
initiating station may retransmit the SABM. This action may be repeated until a response is obtained or action is taken
by a higher layer. If no negotiation procedure is used, DLC parameters are the default one.

5.4.2 DLC Release
The release of a DLC may be initiated by either station by the transmission of a DISC frame with the P-bit set to one.
Confirmation of the DLC release is signalled by the other station sending a UA frame with the F-bit set to 1. Once the
DLC has been released the stations enter disconnected mode for that particular DLC.
If the station receiving the DISC command is already in a disconnected mode it will send a DM response.
If no UA or DM response has been received after T1 the initiating station may retransmit the DISC. This action may be
repeated until a response is obtained or action is taken by a higher layer.

4.3.2.2. Multiplexer Close Down
------------------------------------------
| bit | 1  | 2   | 3 | 4 | 5 | 6 | 7 | 8 |
|     | EA | C/R | 0   0   0   0   1   1 |
------------------------------------------
0xC0

*/

#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <bsp/cellular/bsp_cellular.hpp>

#include "CellularMuxTypes.h"
#include "CellularMuxFrame.h"
#include "MuxParameters.hpp"

#include "DLCChannel.h"
#include "modem/ATParser.hpp"
#include "Service/Service.hpp"

#include <message_buffer.h>

#if defined(__cplusplus)
extern "C"
{
#endif
#include "FreeRTOS.h"
#include "task.h" /* RTOS task related API prototypes. */
#if defined(__cplusplus)
}
#endif

namespace bsp
{
    class Cellular;
}

[[noreturn]] void workerTaskFunction(void *ptr);

class CellularMux
{
  public:
    enum class Channel : unsigned char
    {
        Control       = 0,
        Commands      = 1,
        Notifications = 2,
        Data          = 3,
        None          = 4,
    };
    enum class Mode
    {
        AT,         /// AT raw text mode
        CMUX_SETUP, /// Modem is switching from AT to CMUX
        CMUX        /// multiplexer mode enabled
    };
    enum class ConfState
    {
        Success,
        Failure,
        ModemNeedsReset,
    };
    enum class ControlSignal : uint8_t
    {
        FlowControl           = 0x02,
        ReadyToCommunicate    = 0x04,
        ReadyToReceive        = 0x08,
        IncomingCallIndicator = 0x40,
        DataValid             = 0x80,
    };

  private:
    Mode mode = Mode::AT;

    MuxParameters startParams;
    sys::Service *parentService{};
    std::unique_ptr<bsp::Cellular> cellular;
    std::unique_ptr<ATParser> parser;

    const uint32_t taskPriority = 0;
    xTaskHandle taskHandle      = nullptr;

    std::vector<std::unique_ptr<DLCChannel>> channels;
    DLCChannel::Callback_t controlCallback = nullptr;

    friend void workerTaskFunction(void *ptr);

    enum class EchoCancellerStrength
    {
        LeastAggressive,
        Medium,
        Aggressive,
        Tuned
    };

    friend void workerTaskFunction(void *ptr);
    ConfState setupEchoCanceller(EchoCancellerStrength strength);

    void parseCellularResultCMUX(bsp::cellular::CellularDMAResultStruct &result);
    size_t flushReceiveData();
    void processData(bsp::cellular::CellularDMAResultStruct &result);
    void processError(bsp::cellular::CellularDMAResultStruct &result);
    void sendFrameToChannel(bsp::cellular::CellularResult &resultStruct);

  public:
    CellularMux(PortSpeed_e portSpeed, sys::Service *parent);
    CellularMux() = delete;

    ~CellularMux();

    void setStartParams(PortSpeed_e portSpeed);
    void setMode(Mode mode);

    /// @brief get Channel by index
    /// @param channel enum Channel
    /// @return pointer to channel or nullptr if such channel doesn't exist (nullptr return should never happen how -
    /// because all channels are opened once on start)
    DLCChannel *get(Channel selectedChannel);
    bool openChannel(Channel channelIndex);
    void closeChannels();

    ConfState baudDetectOnce();
    ConfState baudDetectProcedure(uint16_t timeout_s = 30);
    ConfState confProcedure();
    ConfState audioConfProcedure();
    ConfState startMultiplexer();

    MuxParameters getStartParams() const noexcept;
    bsp::Cellular *getCellular();
    [[nodiscard]] const std::unique_ptr<ATParser> &getParser() const;

    /// @brief It is searching the resposne for a string
    ///
    /// @param response - tokenized resposne
    /// @param str - string to search in response
    /// @param numberOfExpectedTokens - number of expected tokens, 0 means do not validate number of tokens
    /// @param level - determine how the errors are logged
    /// @return true - "OK" string is found, false - otherwise
    bool searchATCommandResponse(const std::vector<std::string> &response,
                                 const std::string &str,
                                 size_t numberOfExpectedTokens,
                                 logger_level level);
    bool searchForString(const std::vector<std::string> &response, const std::string &str);

    /// @brief It is serching the resposne for ">" string
    ///
    /// @note Invalid responses are logged by defult as LOG_ERRORs
    ///
    /// @param response - tokenized resposne
    /// @param level - determine how the errors are logged
    /// @return true - str string is found, false - otherwise
    bool checkATCommandPrompt(const std::vector<std::string> &response, logger_level level = LOGERROR);

    void selectAntenna(bsp::cellular::antenna antenna);
    [[nodiscard]] bsp::cellular::antenna getAntenna();

    void informModemHostWakeup();
    bool isModemActive();
    void turnOnModem();
    void resetModem();
    void turnOffModem();
    void enterSleepMode();
    void exitSleepMode();
    void registerCellularDevice();

    [[nodiscard]] auto getLastCommunicationTimestamp() const noexcept -> TickType_t;
    [[nodiscard]] auto isCellularInSleepMode() const noexcept -> bool;

    static std::string name(enum Channel name)
    {
        switch (name) {
        case Channel::None:
            return "None";
        case Channel::Control:
            return "Control";
        case Channel::Commands:
            return "Commands";
        case Channel::Notifications:
            return "Notifications";
        case Channel::Data:
            return "Data";
        }
        return "";
    }
};
