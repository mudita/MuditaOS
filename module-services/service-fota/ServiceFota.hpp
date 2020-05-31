#pragma once

#include "api/FotaServiceAPI.hpp"

#include <service-cellular/ServiceCellular.hpp>

#include <Modem/TS0710/DLC_channel.h>
#include <Modem/TS0710/TS0710.h>
#include <Service/Service.hpp>
#include <Service/Message.hpp>

#include <memory>
#include <sstream>

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

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

        // Invoked when timer ticked
        void TickHandler(uint32_t id) override;

        // Invoked during initialization
        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode /*mode*/) override final
        {
            return sys::ReturnCodes::Success;
        }

        void registerMessageHandlers();

        static const char *serviceName;

      private:
        sys::Message_t handleCellularGetChannelResponseMessage(sys::DataMessage *req, sys::ResponseMessage *response);
        sys::Message_t handleServiceCellularNotifications(sys::DataMessage *req, sys::ResponseMessage *response);
        sys::Message_t handleConfigureAPN(sys::DataMessage *req, sys::ResponseMessage *response);
        sys::Message_t handleConnect(sys::DataMessage *req, sys::ResponseMessage *response);
        sys::Message_t handleHttpGet(sys::DataMessage *req, sys::ResponseMessage *response);
        sys::Message_t handleFotaStart(sys::DataMessage *req, sys::ResponseMessage *response);
        void handleChannelNotifications(std::vector<uint8_t> &data);
        uint32_t connectionTimer = 0;
        //    NotificationMuxChannel::NotificationCallback_t notificationCallback = nullptr;
        void getApnConfiguration();
        void getConfig();
        void getActiveCotext();
        void stopActiveContext();
        void setState();
        bool isHTTPS(const std::string &url) const;
        void normalizeUrl(std::string &url) const;
        /// activate apn context
        std::string prepareQIACT(unsigned char contextId);
        /// configure TCP/IP context
        std::string prepareQICSGPcmd(const APN::Config &apn);
        /// deactivate TCP/IP context
        std::string prepareQIDEACT(unsigned char contextId);
        /// download fota
        std::string prepareQFOTADLcmd(const std::string &url);
        /* prepare AT+QHTTPGET with timeout
         * timeout in seconds, default 20
         */
        std::string prepareQHTTPGET(unsigned int timeout = 20);
        std::string prepareQHTTPREAD(unsigned int timeout = 20);
        std::string prepareQHTTPURL(const std::string &url);
        void setupSSLContext();
        bool openURL(const std::string &url);
        at::Result sendAndLogError(const std::string &msg) const;
        at::Result sendAndLogError(const std::string &msg, uint32_t timeout) const;
        void logIfError(const at::Result &result, const std::string &cmdString) const;
        void parseResponse();
        void parseQIACT(const at::Result &result);
        void parseQIND(const string &message);
        void sendProgress(unsigned int progress, const std::string &receiver);
        void sendFotaFinshed(const std::string &receiver);

        State state              = State ::Idle;
        DLC_channel *dataChannel = nullptr;
        APN::ContextMap contextMap;
        std::string url;
        std::string file;
        std::string receiverServiceName;
        unsigned char currentApnContext = 0;
    };

} // namespace FotaService
