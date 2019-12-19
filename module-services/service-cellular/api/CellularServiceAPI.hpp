/*
 *  @file CellularServiceAPI.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_CELLULARSERVICEAPI_HPP
#define PUREPHONE_CELLULARSERVICEAPI_HPP

#include "../messages/CellularMessage.hpp"
#include "utf8/UTF8.hpp"

class Service;

class CellularServiceAPI {
public:
    static bool DialNumber(sys::Service* serv,const std::string& number);
    static bool AnswerIncomingCall(sys::Service* serv);
    static bool HangupCall(sys::Service* serv);
    static bool SendSMS(sys::Service* Serv, UTF8 number, UTF8 message);
};


#endif //PUREPHONE_CELLULARSERVICEAPI_HPP
