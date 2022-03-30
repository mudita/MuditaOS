// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CellularMessage.hpp"
#include "PacketDataCellularMessage.hpp"

#include <modem/mux/CellularMux.h>
#include <PhoneNumber.hpp>
#include <module-bsp/bsp/cellular/bsp_cellular.hpp>
#include <utf8/UTF8.hpp>

#include <cstdint>
#include <string>

class Service;
namespace sys
{
    class Service;
} // namespace sys

namespace CellularServiceAPI
{
    bool DialNumber(sys::Service *serv, const utils::PhoneNumber &number);
    bool DialEmergencyNumber(sys::Service *serv, const utils::PhoneNumber &number);

    bool AnswerIncomingCall(sys::Service *serv);
    bool HangupCall(sys::Service *serv);
    bool DismissCall(sys::Service *serv, bool addNotificationToDB);
    /*
     * @brief Its calls sercive-cellular for selected SIM IMSI number.
     * @param serv pointer to caller service.
     * @param getFullIMSINumber returned string format. false returns only country code, true returns whole iMSI number.
     * #return IMSI number when succeeds, empty string when fails
     */
    std::string GetIMSI(sys::Service *serv, bool getFullIMSINumber = false);
    /*
     * @brief Its subscribes service-cellular for selected SIM own phone number response messages.
     * @param serv pointer to caller service.
     * @param callback called on response message receive event.
     */
    void SubscribeForOwnNumber(sys::Service *serv, std::function<void(const std::string &)> callback);
    /*
     * @brief Its calls service-cellular for selected SIM own phone number.
     * @param serv pointer to caller service.
     */
    void RequestForOwnNumber(sys::Service *serv);
    /*
     * @brief It calls service-cellulat fo newtwork info
     * @param serv pointer to caller service.
     */
    void GetNetworkInfo(sys::Service *serv);

    /*
     * @brief Get current operator, result in
     * CellularCurrentOperatorNameNotification message on notification channel
     */
    void RequestCurrentOperatorName(sys::Service *serv);
    /*
     * @brief It calls service-cellulat to perform operators scan
     * @param serv pointer to caller service.
     * @param fullInfo for retrieve full info true in other case only list of operators (long name), without duplicates
     *
     */
    void StartOperatorsScan(sys::Service *serv, bool fullInfo = false);

    void SetOperatorAutoSelect(sys::Service *serv);
    void SetOperator(sys::Service *serv,
                     at::response::cops::CopsMode mode,
                     at::response::cops::NameFormat format,
                     const std::string &name);

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
                    CellularMux::Channel channel); /// asynchronous, returns message CellureMessageChannelReady;
    bool GetDataChannel(sys::Service *serv);
    bool GetCSQ(sys::Service *serv, std::string &response);
    bool GetCREG(sys::Service *serv, std::string &response);
    bool GetQNWINFO(sys::Service *serv, std::string &response);
    bool GetAntenna(sys::Service *serv, bsp::cellular::antenna &response);

    bool TransmitDtmfTones(sys::Service *serv, uint32_t digit);

    bool USSDRequest(sys::Service *serv, CellularUSSDMessage::RequestType type, std::string data = "");

    /**
     * @brief get all APNs from phone configuration
     */
    bool GetAPN(sys::Service *serv);
    /**
     * @brief get one APN from phone configuration, connected with ctxid
     */
    bool GetAPN(sys::Service *serv, std::uint8_t contextId);
    /**
     * @brief get first APN with type, from phone configuration
     */
    bool GetAPN(sys::Service *serv, packet_data::APN::APNType type);

    /**
     * @brief set up/change existing APN.
     *
     * Allow to change internal APN, for creating New (not empty) APN
     * should be used NewAPN functionality (to not overwrite internal APN)
     *
     */
    bool SetAPN(sys::Service *serv, packet_data::APN::Config apnConfig);

    /**
     * @brief setup new APN, assigns a new contextId
     * @param serv
     * @param apnConfig
     * @return
     */
    bool NewAPN(sys::Service *serv, packet_data::APN::Config apnConfig);

    /**
     * @brief Call SetAPN with empty APN (delete it)
     * @param serv
     * @param contextId
     * @return
     */
    bool DeleteAPN(sys::Service *serv, std::uint8_t contextId);

    bool SetDataTransfer(sys::Service *serv, packet_data::DataTransfer dt);
    bool GetDataTransfer(sys::Service *serv);
    bool SetVoLTE(sys::Service *serv, bool value);

    bool ChangeModulePowerState(sys::Service *serv, cellular::service::State::PowerState newState);

    bool SetFlightMode(sys::Service *serv, bool flightModeOn);

    bool SetConnectionFrequency(sys::Service *serv, uint8_t connectionFrequency);

    bool CallAudioMuteEvent(sys::Service *serv);

    bool CallAudioUnmuteEvent(sys::Service *serv);

    bool CallAudioLoudspeakerOnEvent(sys::Service *serv);

    bool CallAudioLoudspeakerOffEvent(sys::Service *serv);

}; // namespace CellularServiceAPI
