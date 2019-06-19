
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

#include <stdint.h>
#include "cellular/bsp_cellular.hpp"


class MuxDaemon {
public:

    MuxDaemon();
    ~MuxDaemon();

    int Start();
    int Exit();

private:

    enum class States
    {
        MUX_STATE_OPENING,
        MUX_STATE_INITILIZING,
        MUX_STATE_MUXING,
        MUX_STATE_CLOSING,
        MUX_STATE_OFF,
        MUX_STATES_COUNT // keep this the last
    };

    int SendAT(const char* cmd,uint32_t timeout);

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

    std::unique_ptr<bsp::Cellular> cellular;



    States state = States::MUX_STATE_OPENING;


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

    // some state
    // +CMUX=<mode>[,<subset>[,<port_speed>[,<N1>[,<T1>[,<N2>[,<T2>[,<T3>[,<k>]]]]]]]]
    const static int cmux_mode = 0;
    const static int cmux_subset = 0;
    const static int cmux_port_speed = 5; //115200 baud rate
    const static int cmux_port_speed_default = 5; //115200 baud rate
    const static int cmux_N1 = 127; //lots of modem only support short frame, like quectel 2G modules




};


#endif //PUREPHONE_MUXDAEMON_HPP
