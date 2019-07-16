
/*
 * @file MuxDaemon.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 19.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_MUXDAEMON_HPP
#define PUREPHONE_MUXDAEMON_HPP

#include <vector>
#include "MuxChannel.hpp"
#include <stdint.h>
#include "cellular/bsp_cellular.hpp"
#include "Service/Worker.hpp"
#include "InputSerialWorker.hpp"
#include "GSM0710.hpp"
#include "mutex.hpp"
#include "NotificationMuxChannel.hpp"


class MuxDaemon {

public:

    /**
     * Multiplexer configuration
     */

    const static uint32_t baudRate = 115200;
    const static uint32_t virtualPortsCount = 3;
    const static bool hardwareControlFlowEnable = false;

    /**
     * Multiplexer states
     */
    enum class States {
        MUX_STATE_OPENING,
        MUX_STATE_INITILIZING,
        MUX_STATE_MUXING,
        MUX_STATE_CLOSING,
        MUX_STATE_OFF,
        MUX_STATES_COUNT // keep this the last
    };

    static std::unique_ptr<MuxDaemon> Create(NotificationMuxChannel::NotificationCallback_t callback);

    MuxDaemon(NotificationMuxChannel::NotificationCallback_t callback);

    ~MuxDaemon();

    std::vector<std::string> SendCommandResponse(MuxChannel::MuxChannelType type, const char *cmd, size_t rxCount,
                                                 uint32_t timeout = 1000);

    ssize_t WriteMuxFrame(int channel,
                          const unsigned char *input,
                          int length,
                          unsigned char type);

    inline MuxDaemon::States GetState(){return state;}
    inline void SetState(MuxDaemon::States st){state = st;}

private:

    friend InputSerialWorker;
    friend MuxChannel;

    friend void workerTaskFunction(void *ptr);

    int Start();

    int SendAT(const char *cmd, uint32_t timeout);

    ssize_t WriteSerialCache(unsigned char *input, size_t length);

    int CloseMux();


    /*
* Purpose:  Compares two strings.
*                strstr might not work because WebBox sends garbage before the first OK
*                when it's not needed anymore
* Input:      haystack - string to check
*                length - length of string to check
*                needle - reference string to compare to. must be null-terminated.
* Return:    1 if comparison was success, else 0
*/
    int memstr(
            const char *haystack,
            int length,
            const char *needle);




    constexpr static unsigned char closeChannelCmd[] = {
            static_cast<unsigned char>(MuxDefines::GSM0710_CONTROL_CLD ) |
            static_cast<unsigned char>(MuxDefines::GSM0710_CR),
            static_cast<unsigned char>(MuxDefines::GSM0710_EA ) | (0 << 1)};


    std::unique_ptr<bsp::Cellular> cellular;
    std::unique_ptr<InputSerialWorker> inSerialDataWorker;


    States state = States::MUX_STATE_OPENING;

    std::vector<std::unique_ptr<MuxChannel>> channels;

    cpp_freertos::MutexStandard serOutMutex;

    int uih_pf_bit_received = 0;

    NotificationMuxChannel::NotificationCallback_t callback = nullptr;

};


#endif //PUREPHONE_MUXDAEMON_HPP
