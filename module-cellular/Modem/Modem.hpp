
/*
 * @file Modem.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_MODEM_HPP
#define PUREPHONE_MODEM_HPP

#include <stdint.h>
#include "cellular/bsp_cellular.hpp"

class Modem {
public:

    Modem();
    ~Modem();

    void Init();

    int SendAT(const char* cmd,uint32_t timeout);

private:

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

};


#endif //PUREPHONE_MODEM_HPP
