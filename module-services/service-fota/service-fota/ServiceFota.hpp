// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FotaServiceAPI.hpp"

#include <Modem/TS0710/DLC_channel.h>
#include <Modem/TS0710/TS0710.h>
#include <Result.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <service-cellular/ServiceCellular.hpp>

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

class DLC_channel;
class FotaUrcHandler;

namespace sys
{
    class Timer;
} // namespace sys

namespace FotaService
{

    class Service : public sys::Service
    {

      public:
        enum class State
        {
            Idle,
            Connecting,
            Connected,
            QHTTPREAD,
            FOTAUpdate,
            Failed
        };

        Service();

        ~Service();

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode /*mode*/) override final
        {
            return sys::ReturnCodes::Success;
        }

        void registerMessageHandlers();

        static const char *serviceName;

      private:
        /** Get access to data channel
         */
        sys::MessagePointer handleCellularGetChannelResponseMessage(sys::Message *req);
        /** Do nothing until celular finishes it's startup
         */
        sys::MessagePointer handleServiceCellularNotifications(sys::Message *req);
        sys::MessagePointer handleConfigureAPN(sys::Message *req);
        sys::MessagePointer handleConnect(sys::Message *req);
        sys::MessagePointer handleHttpGet(sys::Message *req);
        /** Send fota update command to modem
         */
        sys::MessagePointer handleFotaStart(sys::Message *req);
        /** Handle URC from modem, support for asynchronious commands
         */
        void handleChannelNotifications(std::string &data);
        /** Handle fota progress notification (in cellular)
         */
        sys::MessagePointer handleRawProgress(sys::Message *req);

        std::unique_ptr<sys::Timer> connectionTimer;
        void getApnConfiguration();
        void getConfig();
        void getActiveCotext();
        void stopActiveContext();
        void setState();
        bool isHTTPS(const std::string &url) const;
        void normalizeUrl(std::string &url) const;
        std::string prepareQIACT(unsigned char contextId);
        std::string prepareQICSGPcmd(const APN::Config &apn);
        std::string prepareQICSGPquery(const APN::Config &apn);
        std::string prepareQIDEACT(unsigned char contextId);
        std::string prepareQFOTADLcmd(const std::string &url);
        std::string prepareQHTTPGET(unsigned int timeout = 20);
        std::string prepareQHTTPREAD(unsigned int timeout = 20);
        std::string prepareQHTTPURL(const std::string &url);
        void setupSSLContext();
        bool openURL(const std::string &url);
        std::shared_ptr<at::Result> sendAndLogError(const std::string &msg) const;
        std::shared_ptr<at::Result> sendAndLogError(const std::string &msg, uint32_t timeout) const;
        void logIfError(const at::Result &result, const std::string &cmdString) const;
        void parseResponse();
        void parseQIACT(const at::Result &result);
        void parseQIND(const std::string &message);
        void sendProgress(unsigned int progress, const std::string &receiver);
        void sendFotaFinshed(const std::string &receiver);

        State state              = State ::Idle;
        DLC_channel *dataChannel = nullptr;
        APN::ContextMap contextMap;
        std::string url;
        std::string file;
        std::string receiverServiceName;
        unsigned char currentApnContext = 0;

        friend FotaUrcHandler;
    };

} // namespace FotaService
