
/*
 * @file SMSRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_SMSRECORD_HPP
#define PUREPHONE_SMSRECORD_HPP

#include <string>
#include "utf8/UTF8.hpp"
#include <stdint.h>

struct SMSRecord {

    uint32_t dbID;
    uint32_t threadID;
    uint32_t contactID;
    uint32_t date;
    uint32_t dateSent;
    uint32_t errorCode;
    UTF8 number;
    UTF8 body;
    uint32_t read;
    //SMSType type;

};


#endif //PUREPHONE_SMSRECORD_HPP
