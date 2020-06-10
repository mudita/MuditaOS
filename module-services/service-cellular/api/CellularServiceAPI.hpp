#pragma once
#include "../messages/CellularMessage.hpp"
#include "utf8/UTF8.hpp"
#include <Modem/TS0710/TS0710.h>

#include <PhoneNumber.hpp>
#include "module-bsp/bsp/cellular/bsp_cellular.hpp"
class Service;

namespace CellularServiceAPI
{
    bool DialNumber(sys::Service *serv, const utils::PhoneNumber &number);
    bool AnswerIncomingCall(sys::Service *serv);
    void HangupCall(sys::Service *serv);
    /*
     * @brief Its calls sercive-cellular for selected SIM IMSI number.
     * @param serv pointer to caller service.
     * @param getFullIMSINumber returned string format. false returns only country code, true returns whole iMSI number.
     * #return IMSI number when succeeds, empty string when fails
     */
    std::string GetIMSI(sys::Service *serv, bool getFullIMSINumber = false);
    /*
     * @brief Its calls sercive-cellular for selected SIM own phone number.
     * @param serv pointer to caller service.
     * #return SIM own number when succeeds, empty string when fails
     */
    std::string GetOwnNumber(sys::Service *serv);
    /*
     * @brief It calls service-cellulat fo newtwork info
     * @param serv pointer to caller service.
     */
    void GetNetworkInfo(sys::Service *serv);
    /*
     * @brief It calls service-cellulat to perform operators scan
     * @param serv pointer to caller service.
     *
     */
    void StartOperatorsScan(sys::Service *serv);
    /*
     * @brief It calls service-cellulat to switch antenna
     * @param serv pointer to caller service.
     * @param antenna selected antenna.
     */
    bool SelectAntenna(sys::Service *serv, bsp::cellular::antenna antenna);

    bool SetScanMode(sys::Service *serv, std::string mode);
    bool GetScanMode(sys::Service *serv);
    bool GetFirmwareVersion(sys::Service *serv, std::string &response);
    bool GetChannel(sys::Service *serv,
                    TS0710::Channel channel); /// asynchronous, returns message CellureMessageChannelReady;
    bool GetDataChannel(sys::Service *serv);
    bool GetCSQ(sys::Service *serv, std::string &response);
    bool GetCREG(sys::Service *serv, std::string &response);
    bool GetQNWINFO(sys::Service *serv, std::string &response);
    bool GetAntenna(sys::Service *serv, bsp::cellular::antenna &response);
}; // namespace CellularServiceAPI
