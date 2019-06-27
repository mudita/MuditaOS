
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


class MuxDaemon {

    friend InputSerialWorker;
    friend MuxChannel;

    friend void workerTaskFunction(void *ptr);

public:

    MuxDaemon();

    ~MuxDaemon();

    int Start();

    int Exit();

private:

    int SendAT(const char *cmd, uint32_t timeout);

    ssize_t WriteMuxFrame(int channel,
                          const unsigned char *input,
                          int length,
                          unsigned char type);

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

    enum class States {
        MUX_STATE_OPENING,
        MUX_STATE_INITILIZING,
        MUX_STATE_MUXING,
        MUX_STATE_CLOSING,
        MUX_STATE_OFF,
        MUX_STATES_COUNT // keep this the last
    };

    /*
     * The following arrays must have equal length and the values must
     * correspond. also it has to correspond to the gsm0710 spec regarding
     * baud id of CMUX the command.
     */
    constexpr static int baud_rates[] = {
            0, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1500000, 2000000, 3000000, 4000000
    };

    constexpr static int quectel_speeds[] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 16, 20, 23, 26
    };

    constexpr static unsigned char closeChannelCmd[] = {
            static_cast<unsigned char>(MuxDefines::GSM0710_CONTROL_CLD ) |
            static_cast<unsigned char>(MuxDefines::GSM0710_CR),
            static_cast<unsigned char>(MuxDefines::GSM0710_EA ) | (0 << 1)};


    std::unique_ptr<bsp::Cellular> cellular;
    std::unique_ptr<InputSerialWorker> inSerialDataWorker;


    States state = States::MUX_STATE_OPENING;

    std::unique_ptr<GSM0710Buffer> inputBuffer=nullptr;
    std::vector<MuxChannel> channels;

    cpp_freertos::MutexStandard serOutMutex;

    int uih_pf_bit_received = 0;

    /**
     * Multiplexer configuration
     */

    uint32_t baudRate = 115200;
    uint32_t virtualPortsCount = 4; // max number of virtual channels supported by EG25
    uint32_t frameSize = 127; // default basic frame size, can be extended to max 32kBytes if needed
    bool hardwareControlFlowEnable = false;
    bool cmuxMode = false;



};


#endif //PUREPHONE_MUXDAEMON_HPP
