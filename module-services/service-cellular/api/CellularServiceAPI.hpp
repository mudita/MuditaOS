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
    /*
     * @brief Its calls sercive-cellular for selected SIM IMSI number.
     * @param serv pointer to caller service.
     * @param getFullIMSINumber returned string format. false returns only country code, true returns whole iMSI number.
     * #return IMSI number when succeeds, empty string when fails
     */
    static std::string GetIMSI(sys::Service *serv, bool getFullIMSINumber = false);
    /*
     * @brief Its calls sercive-cellular for selected SIM own phone number.
     * @param serv pointer to caller service.
     * #return SIM own number when succeeds, empty string when fails
     */
    static std::string GetOwnNumber(sys::Service *serv);
    /*
     * @brief It calls service-cellulat fo newtwork info
     * @param serv pointer to caller service.
     */
    static void GetNetworkInfo(sys::Service *serv);
    /*
     * @brief It calls service-cellulat to perform operators scan
     * @param serv pointer to caller service.
     *
     */
    static void StartOperatorsScan(sys::Service *serv);
    /*
     * @brief It calls service-cellulat to switch antenna
     * @param serv pointer to caller service.
     * @param antenna selected antenna. 0 to select antenna A, 1 to select antenna B
     */
    static bool SelectAntenna(sys::Service *serv, uint8_t antenna);
};


#endif //PUREPHONE_CELLULARSERVICEAPI_HPP
