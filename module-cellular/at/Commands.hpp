// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <vector>
#include <at/Constants.hpp>

namespace at
{
    /// These are commands we use in our code with
    /// at::Cmd structure which consists of:
    /// { command, timeout, last : {sent, response, status } }
    ///
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
        RI_PIN_PULSE_OTHER,         /// Turn on RI pin for other incoming URCs
        URC_DELAY_ON,               /// Enable delay the output of URC indication until ring indicator pulse ends
        URC_UART1,                  /// Route URCs to UART1
        AT_PIN_READY_LOGIC,         /// Configure AP_Ready pin logic ( enable, logic level 1, 200ms )
        CSQ_URC_ON,                 /// Turn on signal strength change URC
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
        ATH,       /// hangup
        CHUP,      /// hangup all calls
        QHUP_BUSY, /// hangup all calls with busy reason
        ATA,       /// (doc): timeout should be possibly set up to 90s
        ATD,       /// setup call
        IPR,       /// set baudrate
        CMUX,      /// setup cmux params
        CFUN,      /// set phone functionality
        CMGS,      /// sms
        QCMGS,
        CREG,       /// network registration status
        QNWINFO,    /// network informations (band etc)
        COPS,       /// operators scan
        SIM_DET,    /// sim detection on/off status (1,0)
        QSIMSTAT,   /// sim insertion / removal notification in URC
        SIM_DET_ON, /// enable sim detection
        SIMSTAT_ON, /// enable sim stat urc
        SET_DEFAULT_SCANMODE,
        SET_DEFAULT_SERVICEDOMAIN,
        SET_SERVICEDOMAIN,
        GET_SERVICEDOMAIN,
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
        CSSN,     /// Supplementary Services - Supplementary Service Notifications
        QICSGP,   /// Configure Parameters of a TCP/IP Context
        QIACT,    /// Activate a PDP Context
        QIDEACT,  /// Deactivate a PDP Context
        CSCA,     /// check SMS Center
        QRXGAIN,  /// Set Downlink Gains of RX
        CLVL,     /// Loudspeaker Volume Level Selection
        QMIC,     /// Set Uplink Gains of MIC
        QNVFR,    /// Quectel command to read NV settings
        QNVFW,    /// Quectel command to write NV settings
        QMBNCFG,  /// Quectel command for MBN files management
        QCFG_IMS, /// Set/Get IP Multimedia Services, get state of VoLTE
        QEEC,     /// Echo cancellation parameters
        RING_URC_OFF,
        RING_URC_ON,
        CSQ_URC_OFF,
        SMS_URC_OFF,
        SMS_URC_ON,
        ACT_URC_OFF,
        ACT_URC_ON,
        SET_RNDIS,
    };

    enum class commadsSet
    {
        modemInit,
        simInit,
        smsInit
    };

    std::vector<AT> getCommadsSet(commadsSet set);

    constexpr auto AtCmdMaxRetries   = 3;
    constexpr auto AtCmdRetryDelayMS = 50;
}; // namespace at
