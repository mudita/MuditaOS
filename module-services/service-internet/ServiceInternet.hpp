#pragma once

#include "api/InternetServiceAPI.hpp"

#include <service-cellular/ServiceCellular.hpp>

#include <Modem/TS0710/DLC_channel.h>
#include <Modem/TS0710/TS0710.h>
#include <Service/Service.hpp>

#include <memory>
#include <sstream>

namespace InternetService
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

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void registerMessageHandlers();

        static const char *serviceName;

      private:
        void handleCellularGetChannelResponseMessage();
        void handleServiceCellularNotifications();
        void handleConfigureAPN();
        void handleConnect();
        void handleHttpGet();
        void handleNotifications();
        void handleFotaStart();
        uint32_t connectionTimer = 0;
        //    NotificationMuxChannel::NotificationCallback_t notificationCallback = nullptr;
        void getApnConfiguration();
        void getConfig();
        void getActiveCotext();
        void stopActiveContext();
        void setState();
        void parseQIACT(const at::Result &result);
        bool isHTTPS(const std::string &url) const;
        void normalizeUrl(std::string &url) const;
        std::string prepareQICSGPcmd(const APN::Config &apn);
        std::string prepareQFOTADLcmd(const std::string &url);
        void setupSSLContext();
        bool openURL(const std::string &url);
        void messageError(const string &msg) const;
        void parseResponse();
        void parseQIND(const string &message);
        void sendProgress(unsigned int progress, const std::string &receiver);
        void sendFotaFinshed(const std::string &receiver);

        State state              = State ::Idle;
        DLC_channel *dataChannel = nullptr;
        APN::ContextMap contextMap;
        std::string url;
        std::string file;
        std::string receiverServiceName;
    };

} // namespace InternetService
