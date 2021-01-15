﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CellularMessage.hpp"
#include "PacketDataCellularMessage.hpp"

#include <Modem/TS0710/TS0710.h>
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
     * @brief Get current operator, result async in
     * CellularGetCurrentOperatorResponse message
     */
    void GetCurrentOperator(sys::Service *serv);
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
                    TS0710::Channel channel); /// asynchronous, returns message CellureMessageChannelReady;
    bool GetDataChannel(sys::Service *serv);
    bool GetCSQ(sys::Service *serv, std::string &response);
    bool GetCREG(sys::Service *serv, std::string &response);
    bool GetQNWINFO(sys::Service *serv, std::string &response);
    bool GetAntenna(sys::Service *serv, bsp::cellular::antenna &response);

    bool TransmitDtmfTones(sys::Service *serv, uint32_t digit);

    bool USSDRequest(sys::Service *serv, CellularUSSDMessage::RequestType type, std::string data = "");

    bool ChangeSimPin(sys::Service *serv,
                      Store::GSM::SIM sim,
                      const std::vector<unsigned int> &passcode,
                      const std::vector<unsigned int> &pin);
    bool SetSimCardLock(sys::Service *serv,
                        Store::GSM::SIM sim,
                        CellularSimCardLockDataMessage::SimCardLock lock,
                        const std::vector<unsigned int> &pin);
    bool SetSimCard(sys::Service *serv, Store::GSM::SIM sim);
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

    bool SetAPN(sys::Service *serv, packet_data::APN::Config apnConfig);
    bool SetDataTransfer(sys::Service *serv, packet_data::DataTransfer dt);
    bool GetDataTransfer(sys::Service *serv);
    bool SetVoLTE(sys::Service *serv, bool value);

    bool ChangeModulePowerState(sys::Service *serv, cellular::State::PowerState newState);
}; // namespace CellularServiceAPI
