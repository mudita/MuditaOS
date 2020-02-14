#pragma once

#include <string>
#include <vector>

#include <FreeRTOS.h>
#include <mutex.hpp>
#include <task.h>

namespace at
{

    namespace cmd
    {
        const inline std::string ECHO_OFF = "ATE0\r";
        const inline std::string FACTORY_RESET = "AT&F\r"; // TODO CHECK IF NEEDED, IT MIGHT BREAK QSIM SETTINGS
        const inline std::string SW_INFO = "ATI\r";
        const inline std::string FLOW_CTRL_ON = "AT+IFC=2,2\r\n";
        const inline std::string FLOW_CTRL_OFF = "AT+IFC=0,0\r";
        const inline std::string URC_NOTIF_CHANNEL = "AT+QCFG=\"cmux/urcport\",1\r";          /// Route URCs to second (Notifications) MUX channel
        const inline std::string RI_PIN_OFF_CALL = "AT+QCFG=\"urc/ri/ring\",\"off\"\r";       /// Turn off RI pin for incoming calls
        const inline std::string RI_PIN_OFF_SMS = "AT+QCFG=\"urc/ri/smsincoming\",\"off\"\r"; /// Turn off RI pin for incoming sms
        const inline std::string URC_UART1 = "AT+QURCCFG=\"urcport\",\"uart1\"\r";            /// Route URCs to UART1
        const inline std::string AT_PIN_READY_LOGIC = "AT+QCFG=\"apready\",1,1,200\r";        /// Configure AP_Ready pin logic ( enable, logic level 1, 200ms )
        const inline std::string URC_NOTIF_SIGNAL = "AT+QINDCFG=\"csq\",1\r";                 /// Turn on signal strength change URC
        const inline std::string CRC_ON = "AT+CRC=1\r";                                       /// Change incoming call notification from "RING" to "+CRING:type"
        const inline std::string CALLER_NUMBER_PRESENTATION = "AT+CLIP=1\r"; /// Turn on caller's number presentation
        const inline std::string SMS_TEXT_FORMAT = "AT+CMGF=1\r";            /// Set Message format to Text
        const inline std::string SMS_UCSC2 = "AT+CSCS=\"UCS2\"\r";           /// Set ucs2 message format
        // unused: const inline std::string FORCE_GSM = "AT+CSCS=\"GSM\"\r";
        const inline std::string SMS_STORAGE = "AT+CPMS=“ME”,“ME”,“ME”"; /// Set SMS prefferd storage
        const inline std::string QSCLK_ON = "AT+QSCLK=1\r";              /// Configure Whether or Not to Enter into Sleep Mode
    };                                                                   // namespace cmd

    struct Result
    {
        /// result class for AT send -> receive command, could return promise :p
        enum class Code
        {
            OK,      // at OK
            ERROR,   // at ERROR
            TIMEOUT, // at Timeout
            TOKENS,  // at numbers of tokens needed met
            NONE,    // no code
        } code = Code::NONE;
        std::vector<std::string> response;
        explicit operator bool() const
        {
            return code == Code::OK;
        }
    };

    class Chanel
    {
      protected:
        cpp_freertos::MutexStandard mutex;
        TaskHandle_t blockedTaskHandle = nullptr;

      public:
        static const std::string OK;
        static const std::string ERROR;
        // /// other codes unused right now: Please see quectel QuectelEC2526EC21ATCommandsManualV1.3 page 21
        // const std::string Chanel::CONNECT = "CONNECT";
        // const std::string Chanel::RING = "RING";
        // const std::string Chanel::NO_CARRIER = "NO CARRIER";
        // const std::string Chanel::NO_DIALTONE = "NO DIALTONE";
        // const std::string Chanel::BUSY = "BUSY";
        // const std::string Chanel::NO_ANSVER = "NO ANSWER";

        /// waits till ok or timeout
        virtual Result cmd(const std::string cmd, uint32_t timeout = 5000, size_t rxCount = 0) final;
        /// check for OK, ERROR in string in last token
        virtual Result::Code at_check(const std::vector<std::string> &arr);
        /// log result
        virtual void cmd_log(std::string cmd, const Result &result, uint32_t timeout) final;

        virtual void cmd_init() = 0;
        virtual void cmd_send(std::string cmd) = 0;
        virtual std::vector<std::string> cmd_receive() = 0;
        virtual void cmd_post() = 0;
    };
}; // namespace at
