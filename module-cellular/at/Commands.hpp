// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

/// This file goal is to provide one place to store all at commands, with their respectable timeouts if these have

#include "Result.hpp"
#include <log/log.hpp>
#include <map>
#include <memory>
#include <string>
#include "Cmd.hpp"
#include "cmd/CSCA.hpp"
#include <chrono>

namespace at
{

    using namespace std::chrono_literals;
    const auto default_doc_timeout      = 5000ms; /// if unsure - take this
    const auto default_long_doc_timeout = 15000ms;

    /// at::Cmd structure with command, it's timeout and some runtime data
    /// { command, timeout, last : {sent, response, status } }

    /// doc is: per Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf
    enum class AT
    {
        AT,
        ECHO_OFF,
        FACTORY_RESET,
        SW_INFO,
        FLOW_CTRL_ON,
        FLOW_CTRL_OFF,
        URC_NOTIF_CHANNEL,          /// Route URCs to second (Notifications) MUX channel
        RI_PIN_AUTO_CALL,           /// Turn on RI pin for incoming calls
        RI_PIN_OFF_CALL,            /// Turn off RI pin for incoming calls
        RI_PIN_PULSE_SMS,           /// Turn on RI pin for incoming sms
        RI_PIN_OFF_SMS,             /// Turn off RI pin for incoming sms
        RI_PIN_OFF_OTHER,           /// Turn off RI pin for other URCs
        URC_DELAY_ON,               /// Enable delay the output of URC indication until ring indicator pulse ends
        URC_UART1,                  /// Route URCs to UART1
        AT_PIN_READY_LOGIC,         /// Configure AP_Ready pin logic ( enable, logic level 1, 200ms )
        URC_NOTIF_SIGNAL,           /// Turn on signal strength change URC
        CRC_ON,                     /// Change incoming call notification from "RING" to "+CRING:type"
        CALLER_NUMBER_PRESENTATION, /// Turn on caller's number presentation
        SMS_TEXT_FORMAT,            /// Set Message format to Text
        SMS_UCSC2,                  /// Set ucs2 message format
        SMS_GSM,                    /// Set gsm message format
        QSCLK_ON,                   /// Configure Whether or Not to Enter into Sleep Mode
        QDAI,                       /// GSM audio initialization check
        QDAI_INIT, /// Audio configuration: custom PCM, 16 bit linear samples, primary mode, 16kHz, master
                   /// Quectel confirmed that during init phase modem sends "ready notification" way before
                   /// audio subsystem is initialized. The only recommended solution for this is to send configuration
                   /// command repetitively until modem responds with OK. Due to our system characteristic we can't use
                   /// here simple while loop with vTaskDelay as this function will be invoked from AudioService
                   /// context. By design service's routines should be as fast as they can and non blocking. Therefore
                   /// there is possibility for audioservice to block for too long waiting in while loop which will
                   /// trigger SystemManager ping/pong failure procedure.
        SET_URC_CHANNEL,
        CSQ,  /// Signal strength query
        CLCC, /// list current calls
        CMGD, /// remove SMS message from GSM memory
        CNUM, /// doc: the command can get the subscribers own number(s) from the (U)SI
        CIMI, /// Its getting IMSI from selected SIM card
        QCMGR,
        ATH,  /// hangup
        ATA,  /// (doc): timeout should be possibly set up to 90s
        ATD,  /// setup call
        IPR,  /// set baudrate
        CMUX, /// setup cmux params
        CFUN, /// set phone functionality
        CMGS, /// sms
        QCMGS,
        CREG,       /// network registration status
        QNWINFO,    /// network informations (band etc)
        COPS,       /// operators scan
        SIM_DET,    /// sim detection on/off status (1,0)
        QSIMSTAT,   /// sim insertion / removal notification in URC
        SIM_DET_ON, /// enable sim detection
        SIMSTAT_ON, /// enable sim stat urc
        SET_SCANMODE,
        GET_SCANMODE,
        QGMR,               /// ditailed firmware revision (as required by Quectel)
        STORE_SETTINGS_ATW, /// required to save in firmware ex SIMSTAT_ON
        CEER,               /// get error description from modem
        QIGETERROR,         /// get tcp/ip error code
        VTS,                /// DTMF and Tone Generation
        QLDTMF,             /// Play Local DTMF
        CUSD_OPEN_SESSION,
        CUSD_CLOSE_SESSION,
        CUSD_SEND,
        SET_SMS_STORAGE,
        CPIN,
        GET_CPIN,
        QPINC, /// Get Pin/Puk attempts. For standard SIM facility (parameter) is "SC"
        CLCK,
        CPWD,
        ENABLE_TIME_ZONE_UPDATE,
        SET_TIME_ZONE_REPORTING,
        DISABLE_TIME_ZONE_UPDATE,
        DISABLE_TIME_ZONE_REPORTING,
        ENABLE_NETWORK_REGISTRATION_URC,
        SET_SMS_TEXT_MODE_UCS2,
        CFUN_RESET,
        CFUN_MIN_FUNCTIONALITY,    /// Set minimum functionality
        CFUN_FULL_FUNCTIONALITY,   /// Full functionality
        CFUN_DISABLE_TRANSMITTING, /// Disable the ME from both transmitting and receiving RF signals
        LIST_MESSAGES,             /// List all messages from message storage
        GET_IMEI,
        CCFC, /// Supplementary Services - Call Forwarding Number and Conditions Control
        CCWA, /// Supplementary Services - Call Waiting Control
        CCWA_GET,
        CHLD, /// Supplementary Services - Call Related Supplementary Services
        CLIP, /// Supplementary Services - Calling Line Identification Restriction
        CLIP_GET,
        CLIR, /// Supplementary Services - Calling Line Identification Restriction
        CLIR_GET,
        CLIR_ENABLE,
        CLIR_DISABLE,
        CLIR_RESET,
        COLP, /// Supplementary Services - Connected Line Identification Presentation
        COLP_GET,
        COLP_ENABLE,
        COLP_DISABLE,
        CSSN,    /// Supplementary Services - Supplementary Service Notifications
        QICSGP,  /// Configure Parameters of a TCP/IP Context
        QIACT,   /// Activate a PDP Context
        QIDEACT, /// Deactivate a PDP Context
        CSCA,    /// check SMS Center
        QRXGAIN, /// Set Downlink Gains of RX
        CLVL,    /// Loudspeaker Volume Level Selection
        QMIC,    /// Set Uplink Gains of MIC
        QEEC,    /// Echo cancellation parameters
    };

    // below timeouts are defined in Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf
    inline auto factory(AT at) -> const Cmd &
    {
        static const std::map<AT, const Cmd> fact{
            {AT::AT, {"AT", 100ms}},
            {AT::ECHO_OFF, {"ATE0"}},
            {AT::FACTORY_RESET, {"AT&F"}},
            {AT::SW_INFO, {"ATI\r", default_doc_timeout}},
            {AT::FLOW_CTRL_ON, {"AT+IFC=2,2\r\n", 500ms}},
            {AT::FLOW_CTRL_OFF, {"AT+IFC=0,0", 500ms}},
            {AT::URC_NOTIF_CHANNEL, {"AT+QCFG=\"cmux/urcport\",1"}},
            {AT::RI_PIN_AUTO_CALL, {"AT+QCFG=\"urc/ri/ring\",\"auto\""}},
            {AT::RI_PIN_OFF_CALL, {"AT+QCFG=\"urc/ri/ring\",\"off\""}},
            {AT::RI_PIN_PULSE_SMS, {"AT+QCFG=\"urc/ri/smsincoming\",\"pulse\",450"}},
            {AT::RI_PIN_OFF_SMS, {"AT+QCFG=\"urc/ri/smsincoming\",\"off\""}},
            {AT::RI_PIN_OFF_OTHER, {"AT+QCFG=\"urc/ri/other\",\"off\""}},
            {AT::URC_DELAY_ON, {"AT+QCFG=\"urc/delay\",1"}},
            {AT::URC_UART1, {"AT+QURCCFG=\"urcport\",\"uart1\""}},
            {AT::AT_PIN_READY_LOGIC, {"AT+QCFG=\"apready\",1,1,200"}},
            {AT::URC_NOTIF_SIGNAL, {"AT+QINDCFG=\"csq\",1"}},
            {AT::CRC_ON, {"AT+CRC=1"}},
            {AT::CALLER_NUMBER_PRESENTATION, {"AT+CLIP=1", 18000ms}},
            {AT::SMS_TEXT_FORMAT, {"AT+CMGF=1"}},
            {AT::SMS_UCSC2, {"AT+CSCS=\"UCS2\""}},
            {AT::SMS_GSM, {"AT+CSCS=\"GSM\""}},
            {AT::QSCLK_ON, {"AT+QSCLK=1", 3000ms}},
            {AT::QDAI, {"AT+QDAI?"}},
            {AT::QDAI_INIT, {"AT+QDAI=1,0,0,3,0,1,1,1"}},
            {AT::SET_URC_CHANNEL, {"AT+QCFG=\"cmux/urcport\",2", default_doc_timeout}},
            {AT::CSQ, {"AT+CSQ", default_doc_timeout}},
            {AT::CLCC, {"AT+CLCC", default_doc_timeout}},
            {AT::CMGD, {"AT+CMGD=", default_doc_timeout}},
            {AT::CNUM, {"AT+CNUM"}},
            {AT::CIMI, {"AT+CIMI"}},
            {AT::QCMGR, {"AT+QCMGR=", 2000ms}},
            {AT::ATH, {"ATH"}},
            {AT::ATA, {"ATA", 90000ms}},
            {AT::ATD, {"ATD"}},
            {AT::IPR, {"AT+IPR="}},
            {AT::CMUX, {"AT+CMUX="}},
            {AT::CFUN, {"AT+CFUN=", default_long_doc_timeout}},
            {AT::CFUN_RESET, {"AT+CFUN=1,1", default_long_doc_timeout}},
            {AT::CFUN_MIN_FUNCTIONALITY, {"AT+CFUN=0", default_long_doc_timeout}},
            {AT::CFUN_FULL_FUNCTIONALITY, {"AT+CFUN=1", default_long_doc_timeout}},
            {AT::CFUN_DISABLE_TRANSMITTING, {"AT+CFUN=4", default_long_doc_timeout}},
            {AT::CMGS, {"AT+CMGS=\"", 120s}},   // verified in docs
            {AT::QCMGS, {"AT+QCMGS=\"", 120s}}, // verified in docs
            {AT::CREG, {"AT+CREG?", default_doc_timeout}},
            {AT::QNWINFO, {"AT+QNWINFO"}},
            {AT::COPS, {"AT+COPS", 180000ms}},
            {AT::QSIMSTAT, {"AT+QSIMSTAT?"}},
            {AT::SIM_DET, {"AT+QSIMDET?"}},
            {AT::SIM_DET_ON, {"AT+QSIMDET=1,0"}},
            {AT::SIMSTAT_ON, {"AT+QSIMSTAT=1"}},
            {AT::SET_SCANMODE, {"AT+QCFG=\"nwscanmode\","}},
            {AT::GET_SCANMODE, {"AT+QCFG=\"nwscanmode\""}},
            {AT::QGMR, {"AT+QGMR"}},
            {AT::STORE_SETTINGS_ATW, {"AT&W"}},
            {AT::CEER, {"AT+CEER"}},
            {AT::QIGETERROR, {"AT+QIGETERROR"}},
            {AT::VTS, {"AT+VTS="}},
            {AT::QLDTMF, {"AT+QLDTMF=1,"}},
            {AT::CUSD_OPEN_SESSION, {"AT+CUSD=1"}},
            {AT::CUSD_CLOSE_SESSION, {"AT+CUSD=2"}},
            {AT::CUSD_SEND, {"AT+CUSD=1,"}},
            {AT::SET_SMS_STORAGE, {"AT+CPMS=\"SM\",\"SM\",\"SM\"", 300ms}},
            {AT::CPIN, {"AT+CPIN=", default_timeout}},
            {AT::GET_CPIN, {"AT+CPIN?", default_timeout}},
            {AT::QPINC, {"AT+QPINC=", default_timeout}},
            {AT::CLCK, {"AT+CLCK=", default_timeout}},
            {AT::CPWD, {"AT+CPWD=", default_timeout}},
            {AT::ENABLE_TIME_ZONE_UPDATE, {"AT+CTZU=3"}},
            {AT::SET_TIME_ZONE_REPORTING, {"AT+CTZR=2"}},
            {AT::DISABLE_TIME_ZONE_UPDATE, {"AT+CTZU=0"}},
            {AT::DISABLE_TIME_ZONE_REPORTING, {"AT+CTZR=0"}},
            {AT::ENABLE_NETWORK_REGISTRATION_URC, {"AT+CREG=2"}},
            {AT::SET_SMS_TEXT_MODE_UCS2, {"AT+CSMP=17,167,0,8"}},
            {AT::LIST_MESSAGES, {"AT+CMGL=\"ALL\"", default_doc_timeout}},
            {AT::GET_IMEI, {"AT+GSN", default_doc_timeout}},
            {AT::CCFC, {"AT+CCFC=", default_doc_timeout}},
            {AT::CCWA, {"AT+CCWA=", default_doc_timeout}},
            {AT::CCWA_GET, {"AT+CCWA?", default_doc_timeout}},
            {AT::CHLD, {"AT+CHLD=\"", default_doc_timeout}},
            {AT::CLIP, {"AT+CLIP=", default_long_doc_timeout}},
            {AT::CLIP_GET, {"AT+CLIP?", default_long_doc_timeout}},
            {AT::CLIR, {"AT+CLIR=", default_long_doc_timeout}},
            {AT::CLIR_GET, {"AT+CLIR?", default_long_doc_timeout}},
            {AT::CLIR_RESET, {"AT+CLIR=0", default_long_doc_timeout}},
            {AT::CLIR_ENABLE, {"AT+CLIR=1", default_long_doc_timeout}},
            {AT::CLIR_DISABLE, {"AT+CLIR=2", default_long_doc_timeout}},
            {AT::COLP, {"AT+COLP", default_long_doc_timeout}},
            {AT::COLP_GET, {"AT+COLP?", default_long_doc_timeout}},
            {AT::COLP_ENABLE, {"AT+COLP=1", default_long_doc_timeout}},
            {AT::COLP_DISABLE, {"AT+COLP=0", default_long_doc_timeout}},
            {AT::CSSN, {"AT+CSSN=\"", default_doc_timeout}},
            {AT::QICSGP, {"AT+QICSGP", default_timeout}},
            {AT::QIACT, {"AT+QIACT", 150000ms}},
            {AT::QIDEACT, {"AT+QIDEACT", 40000ms}},
            {AT::QRXGAIN, {"AT+QRXGAIN=40000", default_timeout}},
            {AT::CLVL, {"AT+CLVL=3", default_timeout}},
            {AT::QMIC, {"AT+QMIC=15000,15000", default_timeout}},
            {AT::QEEC, {"AT+QEEC=", default_timeout}}};

        if (fact.count(at) != 0u) {
            return fact.at(at);
        }
        LOG_ERROR("NO SUCH AT COMMAND DEFINED: %d", static_cast<int>(at));
        return fact.at(AT::AT);
    }

    enum class commadsSet
    {
        modemInit,
        simInit,
        smsInit
    };

    std::vector<AT> getCommadsSet(commadsSet set);
}; // namespace at
