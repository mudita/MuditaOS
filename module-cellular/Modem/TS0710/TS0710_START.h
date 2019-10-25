/**
 * Project Untitled
 */


#ifndef _TS0710_START_H
#define _TS0710_START_H

#include <tuple>
#include <map>
#include "DLC_channel.h"

class TS0710_START {
public: 

enum class Mode_e { Basic, HDLC_UIH_frames, HDLC_UI_frames, HDLC_frames }; 
//enum class PortSpeed_e { PS2400, PS4800, PS7200, PS9600, PS19200, PS38400, PS57600, PS115200, PS230400, PS460800 };
 
struct START_SystemParameters_t {
    PortSpeed_e PortSpeed;          //!< Port speed
    int MaxFrameSize;               //!< Maximum Frame Size [ 1-128 in Basic mode, 1-512 in HDLC modes, default: 31 for basic option & 64 for advanced ]
    int AckTimer;                   //!< Acknowledgement Timer [0,01s-2,55s, default: 0,1s]
    int MaxNumOfRetransmissions;    //!< Maximum number of retransmissions [0-100, default: 3]
    int MaxCtrlRespTime;            //!< Response timer for the multiplexer control channel [0,01s-2,55s, default: 0,3s]
    int WakeUpRespTime;             //!< Wake up response timer [1s-255s, default: 10s]
    int ErrRecovWindowSize;         //!< Window size for error recovery mode [1-7, default: 2]
};
// std::map<PortSpeed_e, int> QuectelCMUXPortSpeeds_text = { {PortSpeed_e::PS9600, 1}, {PortSpeed_e::PS19200, 2}, {PortSpeed_e::PS38400, 3}, {PortSpeed_e::PS57600, 4}, {PortSpeed_e::PS115200, 5}, {PortSpeed_e::PS230400, 6}, {PortSpeed_e::PS460800, 7} };
// std::map<PortSpeed_e, int> ATPortSpeeds_text = { {PortSpeed_e::PS9600, 9600}, {PortSpeed_e::PS19200, 19200}, {PortSpeed_e::PS38400, 38400}, {PortSpeed_e::PS57600, 57600}, {PortSpeed_e::PS115200, 115200}, {PortSpeed_e::PS230400, 230400}, {PortSpeed_e::PS460800, 460800} };
// std::map<PortSpeed_e, speed_t> LinuxTermPortSpeeds_text = { {PortSpeed_e::PS9600, B9600}, {PortSpeed_e::PS19200, B19200}, {PortSpeed_e::PS38400, B38400}, {PortSpeed_e::PS57600, B57600}, {PortSpeed_e::PS115200, B115200}, {PortSpeed_e::PS230400, B230400}, {PortSpeed_e::PS460800, B460800} };

private:
    Mode_e pv_mode;
    START_SystemParameters_t pv_system_parameters;
    bool connStatus = false;        //!< Connection status - false: GSM0710 not started, true: client responded
    DLC_channel *ctrlChannel;
    bsp::Cellular *pv_cellular;
    
public:
TS0710_START(Mode_e mode, START_SystemParameters_t system_parameters, bsp::Cellular *cellular);
~TS0710_START();

private:
/**
 * @param mode
 * @param system_parameters
 * @brief The request primitive is used to request that the multiplexer mode to be turned on in the desired mode and system parameters.
 */
void request(Mode_e mode, START_SystemParameters_t system_parameters);
    
/**
 * @param mode
 * @param system_parameters
 * @brief The indication primitive transfers the request to start multiplexer operation along with the desired mode and system parameters to the upper layer of the target device.
 */
//void indication(Mode_e mode, START_SystemParameters_t system_parameters);
    
/**
 * @param mode
 * @param system_parameters
 * @param accept
 * @brief If the target device accepts the request by issuing an affirmative response primitive, the suggested mode and system parameters will become valid. \
 * A successful establishment of the multiplexer mode is indicated by the accept parameter being set to “true”. \
 * If the accept parameter is set to “false” the returned values for the other parameters are those suggested by the responding device.
 */
//void response(Mode_e mode, START_SystemParameters_t system_parameters, bool accept);
    
/**
 * @param mode
 * @param system_parameters
 * @param accept
 * @brief The confirm primitive is returned to the upper layer of the requesting device. \
 * A successful establishment of the multiplexer mode is indicated by the accept parameter being set to “true”. \
 * If the accept parameter is set to “false” the returned values for the other parameters are those suggested by the responding device.
 */
//void confirm(Mode_e mode, START_SystemParameters_t system_parameters, bool accept);
public:
    bool ConnectionStatus() { return connStatus; }
    DLC_channel *getCtrlChannel() { return ctrlChannel; }
};

#endif //_TS0710_START_H