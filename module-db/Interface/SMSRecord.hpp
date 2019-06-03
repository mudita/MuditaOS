
/*
 * @file SMSRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_SMSRECORD_HPP
#define PUREPHONE_SMSRECORD_HPP

#include "Record.hpp"
#include <stdint.h>
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"
#include "../Databases/SmsDB.hpp"

class SMSRecord : public Record {
public:

    SMSRecord();
    ~SMSRecord();

    bool Add() override final;
    bool Remove() override final;
    bool Update() override final;

    uint32_t dbID;
    uint32_t threadID;
    uint32_t contactID;
    uint32_t date;
    uint32_t dateSent;
    uint32_t errorCode;
    UTF8 number;
    UTF8 body;
    uint32_t read;
    SMSType type;

private:

};


#endif //PUREPHONE_SMSRECORD_HPP
