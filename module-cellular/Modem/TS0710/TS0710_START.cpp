/**
 * Project Untitled
 */

#include "TS0710_START.h"
#include "DLC_channel.h"
#include "log/log.hpp"

/**
 * TS0710_START implementation
 */

TS0710_START::TS0710_START(Mode_e mode, START_SystemParameters_t system_parameters, bsp::Cellular *cellular)
{
    pv_mode              = mode;
    pv_system_parameters = system_parameters;
    pv_cellular          = cellular;

    // assemble frame, send request, report connection status
    request(pv_mode, pv_system_parameters);
}

TS0710_START::~TS0710_START()
{}

/**
 * @param mode
 * @param system_parameters
 */
void TS0710_START::request(Mode_e mode, START_SystemParameters_t system_parameters)
{
    // 1. start CMUX by AT command AT+CMUX=...(with given parameters) & get response
    /*
    AT+CMUX=<mode>[,<subset>[,<portspeed>[,<N1>[,<T1>[,<N2>[,<T2>[,<T3>[,<k>]]]]]]]]
    <mode> 0 Basic option.
    <subset> This parameter defines the way in which the MUX control channel is set up.
        Virtual channel may subsequently be set up differently, but in the absence of any
        negotiation for the settings of a virtual channel, it shall be set up according to thecontrol channel <subset>
    setting. 0 UIH frames used only. 1 UI frames used only. 2 I frames used only. <portspeed> 1 9600bit/s 2 19200bit/s
        3 38400bit/s
        4 57600bit/s
        5 115200bit/s
        6 230400bit/s
        7 460800bit/s
    <N1> Maximum frame size, the range is 1-32768, 127 is the default value for basic option (see <mode>).
    <T1> The time UE waits for an acknowledgement before resorting to other action (e.g.
        transmitting a frame).The step size is ten milliseconds, the range is 1-255. The default value is 100ms.
    <N2> Maximum number of re-transmissions, the range is 0-255, the default value is 3.
    <T2> Response timer for MUX control channel, the step size is ten milliseconds, the range is 2-255, 30 is the
    default value. <T3> Wake up response timer in seconds. The range is 1-255, 10 is the default value. <k> Window size
    (It is not supported for UC20)

    PortSpeed_e PortSpeed;          //!< Port speed
    int MaxFrameSize;               //!< Maximum Frame Size [ 1-128 in Basic mode, 1-512 in HDLC modes, default: 31 for
    basic option & 64 for advanced ] int AckTimer;                   //!< Acknowledgement Timer [0,01s-2,55s, default:
    0,1s] int MaxNumOfRetransmissions;    //!< Maximum number of retransmissions [0-100, default: 3] int
    MaxCtrlRespTime;            //!< Response timer for the multiplexer control channel [0,01s-2,55s, default: 0,3s] int
    WakeUpRespTime;             //!< Wake up response timer [1s-255s, default: 10s] int ErrRecovWindowSize;         //!<
    Window size for error recovery mode [1-7, default: 2]
    */

    // 2. Create channel DLCI0 - control
    ctrlChannel = new DLC_channel(0, std::string("Control"), pv_cellular);

    // wait & check for active flag in channel
    if (ctrlChannel->getActive()) {
        connStatus = true;
    }
}
