#pragma once

/// This file goal is to provide one place to store all at commands, with their respectable timeouts if these have

#include "Result.hpp"
#include <log/log.hpp>
#include <map>
#include <memory>
#include <string>

namespace at
{
    inline const uint32_t default_timeout = 5000; /// if unsure - take this
    inline const uint32_t default_doc_timeout =
        300; /// if you've checked it's ok - or it was at least 300 in code somewhere, take this

    /// at::Cmd structure with command, it's timeout and some runtime data
    /// { command, timeout, last : {sent, response, status } }
    struct Cmd
    {
        std::string cmd;                    /// command to run
        uint32_t timeout = default_timeout; /// timeout for this command
        struct
        {
            Result::Code status = Result::Code::NONE; /// last response for that command
            time_t requested    = 0;                  /// last time comand was requested
            time_t response     = 0;                  /// last time command was received
            auto request_time() -> time_t
            {
                return response - requested;
            }   /// time it took to send command and get response
        } last; /// last status of command execution

        Cmd(std::string cmd, uint32_t timeout = default_timeout) : cmd(std::move(cmd)), timeout(timeout)
        {}
        /// not the prettiest, for now it's ok - for commands which modify strings to execute - return copy of command
        /// str
        operator std::string() const
        {
            return cmd;
        }
        auto operator+(const std::string &val) const -> Cmd
        {
            Cmd tmp = *this;
            tmp.cmd += val;
            return tmp;
        }
    };

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
        RI_PIN_OFF_CALL,            /// Turn off RI pin for incoming calls
        RI_PIN_OFF_SMS,             /// Turn off RI pin for incoming sms
        URC_UART1,                  /// Route URCs to UART1
        AT_PIN_READY_LOGIC,         /// Configure AP_Ready pin logic ( enable, logic level 1, 200ms )
        URC_NOTIF_SIGNAL,           /// Turn on signal strength change URC
        CRC_ON,                     /// Change incoming call notification from "RING" to "+CRING:type"
        CALLER_NUMBER_PRESENTATION, /// Turn on caller's number presentation
        SMS_TEXT_FORMAT,            /// Set Message format to Text
        SMS_UCSC2,                  /// Set ucs2 message format
        SMS_STORAGE,                /// Set SMS preferred storage
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
        CMGS, /// sms
        QCMGS,
        CREG,       /// networ registration status
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

    };

    inline auto factory(AT at) -> const Cmd &
    {
        static const std::map<AT, const Cmd> fact{
            {AT::AT, {"AT\r", 100}},
            {AT::ECHO_OFF, {"ATE0\r"}},
            {AT::FACTORY_RESET, {"AT&F\r"}},
            {AT::SW_INFO, {"ATI\r", default_doc_timeout}},
            {AT::FLOW_CTRL_ON, {"AT+IFC=2,2\r\n", 500}},
            {AT::FLOW_CTRL_OFF, {"AT+IFC=0,0\r", 500}},
            {AT::URC_NOTIF_CHANNEL, {"AT+QCFG=\"cmux/urcport\",1\r"}},
            {AT::RI_PIN_OFF_CALL, {"AT+QCFG=\"urc/ri/ring\",\"off\"\r"}},
            {AT::RI_PIN_OFF_SMS, {"AT+QCFG=\"urc/ri/smsincoming\",\"off\"\r"}},
            {AT::URC_UART1, {"AT+QURCCFG=\"urcport\",\"uart1\"\r"}},
            {AT::AT_PIN_READY_LOGIC, {"AT+QCFG=\"apready\",1,1,200\r"}},
            {AT::URC_NOTIF_SIGNAL, {"AT+QINDCFG=\"csq\",1\r"}},
            {AT::CRC_ON, {"AT+CRC=1\r"}},
            {AT::CALLER_NUMBER_PRESENTATION, {"AT+CLIP=1\r", 18000}},
            {AT::SMS_TEXT_FORMAT, {"AT+CMGF=1\r"}},
            {AT::SMS_UCSC2, {"AT+CSCS=\"UCS2\"\r"}},
            {AT::SMS_STORAGE, {"AT+CPMS=\"SM\",\"SM\",\"SM\"\r"}},
            {AT::QSCLK_ON, {"AT+QSCLK=1\r", 3000}},
            {AT::QDAI, {"AT+QDAI?\r"}},
            {AT::QDAI_INIT, {"AT+QDAI=1,0,0,5,0,1\r"}},
            {AT::SET_URC_CHANNEL, {"AT+QCFG=\"cmux/urcport\",2\r", default_doc_timeout}},
            {AT::CSQ, {"AT+CSQ\r", default_doc_timeout}},
            {AT::CLCC, {"AT+CLCC\r", default_doc_timeout}},
            {AT::CMGD, {"AT+CMGD=", default_doc_timeout}},
            {AT::CNUM, {"AT+CNUM\r"}},
            {AT::CIMI, {"AT+CIMI\r"}},
            {AT::QCMGR, {"AT+QCMGR=", 2000}},
            {AT::ATH, {"ATH\r"}},
            {AT::ATA, {"ATA\r", 90000}},
            {AT::ATD, {"ATD"}},
            {AT::IPR, {"AT+IPR="}},
            {AT::CMUX, {"AT+CMUX="}},
            {AT::CMGS, {"AT+CMGS=\""}},
            {AT::QCMGS, {"AT+QCMGS=\""}},
            {AT::CREG, {"AT+CREG?\r", default_doc_timeout}},
            {AT::QNWINFO, {"AT+QNWINFO\r"}},
            {AT::COPS, {"AT+COPS=?\r", 180000}},
            {AT::QSIMSTAT, {"AT+QSIMSTAT?\r"}},
            {AT::SIM_DET, {"AT+QSIMDET?\r"}},
            {AT::SIM_DET_ON, {"AT+QSIMDET=1,0\r"}},
            {AT::SIMSTAT_ON, {"AT+QSIMSTAT=1\r"}},
            {AT::SET_SCANMODE, {"AT+QCFG=\"nwscanmode\","}},
            {AT::GET_SCANMODE, {"AT+QCFG=\"nwscanmode\"\r"}},
            {AT::QGMR, {"AT+QGMR\r"}},
            {AT::STORE_SETTINGS_ATW, {"AT&W\r"}},
            {AT::CEER, {"AT+CEER\r"}},

        };
        if (fact.count(at)) {
            return fact.at(at);
        }
        LOG_ERROR("NO SUCH AT COMMAND DEFINED: %d", static_cast<int>(at));
        return fact.at(AT::AT);
    }
    enum class commadsSet
    {
        modemInit,
        simInit
    };
    std::vector<AT> getCommadsSet(commadsSet set);
}; // namespace at
