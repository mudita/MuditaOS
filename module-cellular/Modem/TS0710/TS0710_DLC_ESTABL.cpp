/**
 * Project Untitled
 */

#include "TS0710_DLC_ESTABL.h"

#if defined(__cplusplus)
extern "C"
{
#endif
//#include "uart.h"
#if defined(__cplusplus)
}
#endif

#include "TS0710_Frame.h"
#include "TS0710_types.h"
#include "log/log.hpp"
#include "FreeRTOS.h"

/**
 * TS0710_DLC_ESTABL implementation
 */
TS0710_DLC_ESTABL::TS0710_DLC_ESTABL(DLCI_t DLCI, bsp::Cellular *cellular)
{
    DLC_ESTABL_SystemParameters_t system_parameters;
    system_parameters.TypeOfFrame             = TypeOfFrame_e::UIH;
    system_parameters.ConvergenceLayer        = 1;
    system_parameters.Priority                = 1;
    system_parameters.AckTime                 = 100; // 100ms default
    system_parameters.MaxFrameSize            = 128;
    system_parameters.MaxNumOfRetransmissions = 3; // default 3
    system_parameters.ErrRecovWindowSize      = 2; // default 2

    pv_DLCI             = DLCI;
    pv_SystemParameters = system_parameters;

    pv_cellular = cellular;

    active = request(DLCI, system_parameters);
}

/**
 * @param DLCI
 * @param system_parameters
 */
bool TS0710_DLC_ESTABL::request(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters)
{
    /*
    TE requests to establish control channel DLCI 0 by SABM frame
        TE→MS   F9 03 3F 01 1C F9     SABM Frame
    MS feeds back UA for receiving SABM and accepts to create DLCI 0
        TE←MS   F9 03 73 01 D7 F9     UA Frame
    */

    /*
    [send data]
         |<--------------
         V              |
     -(is resp.)-       |
     |T        F|       |
     |     -(is t-out)- |
     |     |T        F|-|
     |     |
     |     V
     V {no resp.}
   {resp.}
    */

    /* data flow :
      TX: (data) -> queue[i] -> [ass. frame] -> [send to UART - mutex protected]
          - frame type depends of data source - control/data[dlci]
      RX: [receive from UART] -> [analyze frame] -> queue[i] -> (data)
          - data sent to different queues - control/data[dlci]
    */
    LOG_DEBUG("Sending %s frame to DLCI %i", TypeOfFrame_text[system_parameters.TypeOfFrame].c_str(), DLCI);
    TS0710_Frame::frame_t frame;
    frame.Address = static_cast<uint8_t>(DLCI << 2) | (1 << 1); // set C/R = 1 - command
    frame.Control = static_cast<uint8_t>(system_parameters.TypeOfFrame);
    TS0710_Frame frame_c(frame);
    pv_cellular->Write(static_cast<void *>(frame_c.getSerData().data()), frame_c.getSerData().size());
    // return true;
    int retries = system_parameters.MaxNumOfRetransmissions;
    while (retries--) {
        // UartSend(frame_c.getSerData().data(), frame_c.getSerData().size());
        pv_cellular->Write(static_cast<void *>(frame_c.getSerData().data()), frame_c.getSerData().size());
        vTaskDelay(system_parameters.AckTime);
        if (response(DLCI, system_parameters)) {
            LOG_DEBUG("Got response");
            return true;
        }
    }

    LOG_ERROR("Sending frame failed");
    return false;
}

/**
 * @param DLCI
 * @param system_parameters
 */
void TS0710_DLC_ESTABL::indication(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters)
{}

/**
 * @param DLCI
 * @param system_parameters
 * @param accept
 */
bool TS0710_DLC_ESTABL::response(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters)
{
    constexpr size_t size = 256;
    std::unique_ptr<uint8_t[]> data(new uint8_t[size]);

    // uint32_t len = UartReceive(data);
    ssize_t len = pv_cellular->Read(reinterpret_cast<void *>(data.get()), size);
    LOG_DEBUG("RX length = %d", static_cast<int>(len));

    if (len > 0) {
        std::vector<uint8_t> v(data.get(), data.get() + len);
        TS0710_Frame frame_c(v);
        TS0710_Frame::frame_t frame = frame_c.getFrame();

        if (((frame.Address & 0xFC) == (DLCI << 2)) &&
            (frame.Control == (static_cast<uint8_t>(TypeOfFrame_e::UA) & ~(1 << 4)))) {
            LOG_DEBUG("Frame correct");
            return true;
        }
    }

    LOG_DEBUG("ERROR - discarding frame !");
    return false;
}

/**
 * @param DLCI
 * @param system_parameters
 * @param accept
 */
bool TS0710_DLC_ESTABL::confirm(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t system_parameters)
{
    return false;
}
