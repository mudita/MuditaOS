#include "api/InternetServiceAPI.hpp"

#include "ServiceInternet.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include <module-cellular/Modem/TS0710/TS0710.h>
#include <module-cellular/at/Result.hpp>
#include <service-cellular/api/CellularServiceAPI.hpp>

#include "MessageType.hpp"

#include "messages/InternetMessage.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <sstream>

namespace InternetService
{

    const char *Service::serviceName = "ServiceInternet";

    Service::Service() : sys::Service(serviceName)
    {
        LOG_INFO("[ServiceInternet] Initializing");

        busChannels.push_back(sys::BusChannels::ServiceInternetNotifications);
        busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);

        connectionTimer = CreateTimer(1000, true);
        registerMessageHandlers();
    }

    Service::~Service()
    {
        LOG_INFO("[ServiceInternet] Cleaning resources");
    }

    // Invoked when timer ticked
    void Service::TickHandler(uint32_t /*id*/)
    {
        std::shared_ptr<InternetRequestMessage> msg =
            std::make_shared<InternetRequestMessage>(MessageType::CellularListCurrentCalls);

        sys::Bus::SendUnicast(msg, Service::serviceName, this);
    }

    // Invoked during initialization
    sys::ReturnCodes Service::InitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes Service::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void Service::registerMessageHandlers()
    {
        LOG_DEBUG("Registring Handlers for ServiceInternet:");
        handleServiceCellularNotifications();
        handleCellularGetChannelResponseMessage();
        handleConfigureAPN();
        handleConnect();
    }

    void Service::handleCellularGetChannelResponseMessage()
    {
        LOG_DEBUG("  -CellularGetChannelResponseMessage");
        connect(CellularGetChannelResponseMessage(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
            LOG_DEBUG("Handling channel response");
            auto responseMessage = static_cast<CellularGetChannelResponseMessage *>(req);
            LOG_DEBUG("chanel ptr: %p", responseMessage->dataChannelPtr);
            dataChannel = responseMessage->dataChannelPtr;
            getApnConfiguration();
            setState();
            return sys::Message_t();
        });
    }

    void Service::handleServiceCellularNotifications()
    {
        connect(cellular::StateChange(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
            if (auto msg = dynamic_cast<cellular::StateChange *>(req)) {
                LOG_DEBUG("cellular::StageChange: %s", cellular::State::c_str(msg->request));
                if (msg->request == cellular::State::ST::Ready) {
                    LOG_DEBUG("Modem is Idle");
                    if (dataChannel == nullptr) {
                        LOG_DEBUG("Requesting channel");
                        CellularServiceAPI::GetDataChannel(this);
                    }
                    else {
                        LOG_DEBUG("Channel already present: %p", dataChannel);
                    }
                }
            }
            return std::make_shared<sys::ResponseMessage>();
        });
    }

    void Service::handleConfigureAPN()
    {
        connect(ConfigureAPNMessage(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
            if (dataChannel) {
                if (auto msg = dynamic_cast<ConfigureAPNMessage *>(req)) {
                    LOG_DEBUG("Configure APN: %s", msg->apnConfig.toString().c_str());
                    auto config = std::find_if(
                        contextMap.begin(), contextMap.end(), [&msg](const APN::ContextPair &pair) -> bool {
                            return pair.second.contextId == msg->apnConfig.contextId;
                        });

                    at::Result atResult;

                    if (config == contextMap.end()) {
                        LOG_DEBUG("Configuring new APN: %s", msg->apnConfig.toString().c_str());
                        atResult = dataChannel->cmd(prepareQICSGPcmd(msg->apnConfig));
                    }
                    else {
                        if (config->second.contextId == msg->apnConfig.contextId) {
                            if (config->second.activated && (config->second.apn != msg->apnConfig.apn)) {
                                atResult = dataChannel->cmd(
                                    "AT+QIDEACT=" + std::to_string(config->second.contextId) + "\r", 40000);
                                if (!atResult) {
                                    auto errData = dataChannel->cmd(at::AT::QIGETERROR);
                                    LOG_DEBUG("deactivation error! %s",
                                              std::accumulate(
                                                  errData.response.begin(), errData.response.end(), std::string(""))
                                                  .c_str());
                                }
                            }
                            atResult = dataChannel->cmd(prepareQICSGPcmd(msg->apnConfig));
                        }
                        else {
                            atResult.code = at::Result::Code::OK;
                        }
                    }

                    std::shared_ptr<sys::ResponseMessage> responseMsg;
                    if (atResult) {
                        responseMsg = std::make_shared<InternetResponseMessage>(true);
                        sys::Bus::SendMulticast(
                            std::make_shared<NotificationMessage>(NotificationMessage::Type::Configured),
                            sys::BusChannels::ServiceInternetNotifications,
                            this);
                        LOG_DEBUG("Internet Cofiguration OK");
                    }
                    else {
                        responseMsg  = std::make_shared<InternetResponseMessage>(true);
                        auto errData = dataChannel->cmd(at::AT::QIGETERROR);
                        LOG_DEBUG("Internet Configuration Failed: %s",
                                  (std::accumulate(errData.response.begin(), errData.response.end(), std::string("")))
                                      .c_str());
                    }
                    return responseMsg;
                }
            }
            return std::make_shared<sys::ResponseMessage>();
        });
    }

    void Service::handleConnect()
    {
        connect(ConnectMessage(), [&](sys::DataMessage * /*req*/, sys::ResponseMessage * /*response*/) {
            std::shared_ptr<sys::ResponseMessage> responseMsg;
            if (dataChannel) {
                LOG_DEBUG("ConnectMessage");
                if (dataChannel->cmd("AT+QIACT=1\r", 150000)) {
                    sys::Bus::SendMulticast(
                        std::make_shared<NotificationMessage>(
                            static_cast<NotificationMessage::Type>(NotificationMessage::Type::Connected)),
                        sys::BusChannels::ServiceInternetNotifications,
                        this);
                    LOG_DEBUG("InternetConnect OK");
                    responseMsg = std::make_shared<InternetResponseMessage>(true);
                }
                else {
                    auto errData = dataChannel->cmd(at::AT::QIGETERROR);
                    LOG_DEBUG(
                        "InternetConnect Fail: %s",
                        std::accumulate(errData.response.begin(), errData.response.end(), std::string("")).c_str());
                    responseMsg = std::make_shared<InternetResponseMessage>(false);
                }
            }
            return responseMsg;
        });
    }

    void Service::getApnConfiguration()
    {
        getActiveCotext();
        getConfig();
    }

    void Service::getConfig()
    {
        if (dataChannel) {
            for (auto &[contextId, apn] : contextMap) {
                if (auto data = dataChannel->cmd("AT+QICSGP=" + std::to_string(contextId) + "\r")) {
                    const std::string QICSGP_prefix("+QICSGP:");
                    for (auto &line : data.response) {
                        if (line.find(QICSGP_prefix) != std::string::npos) {
                            istringstream raw_data(line.substr(QICSGP_prefix.size()));
                            std::vector<std::string> data;
                            std::string subitem;
                            while (std::getline(raw_data, subitem, ',')) {
                                LOG_DEBUG("   %s: %s", QICSGP_prefix.c_str(), subitem.c_str());
                                data.push_back(subitem);
                            }
                            APN::ContextType configuredType = static_cast<APN::ContextType>(std::stoi(data[0]));
                            if (apn.type != configuredType) {
                                LOG_INFO("Different content type active vs configured-> %d vs %d",
                                         static_cast<int>(apn.type),
                                         static_cast<int>(configuredType));
                            }
                            apn.apn        = data[1].substr(1, data[1].size() - 1);
                            apn.username   = data[2].substr(1, data[2].size() - 1);
                            apn.password   = data[3].substr(1, data[3].size() - 1);
                            apn.authMethod = static_cast<APN::AuthMethod>(std::stoi(data[4]));
                        }
                    }
                }
            }
        }
    }

    void Service::getActiveCotext()
    {
        if (dataChannel) {
            auto availableContext = dataChannel->cmd("AT+QIACT?\r");
            if (availableContext) {
                parseQIACT(availableContext);
            }
        }
    }

    void Service::setState()
    {
        for (auto &[contextId, apn] : contextMap) {
            if (apn.activated) {
                state = State::Connected;
            }
        }
    }

    void Service::parseQIACT(const at::Result &availableContext)
    {
        const std::string QIACT_prefix("+QIACT:");
        for (auto &line : availableContext.response) {
            LOG_DEBUG("context: %s", line.c_str());
            if (line.find(QIACT_prefix) != std::string::npos) {
                APN::Config apnConfig;
                std::vector<std::string> data;
                istringstream raw_data(line.substr(QIACT_prefix.size()));
                std::string subItem;
                while (std::getline(raw_data, subItem, ',')) {
                    LOG_DEBUG("QIACT-: %s", subItem.c_str());
                    data.push_back(subItem);
                }
                apnConfig.contextId = static_cast<unsigned char>(std::stoi(data[0]));
                apnConfig.activated = (data[1] == "1" ? true : false);
                int contextTypeRaw  = std::stoi(data[2]);
                apnConfig.type      = static_cast<APN::ContextType>(contextTypeRaw);
                apnConfig.ip        = data[3].substr(1, data[3].size() - 1);
                LOG_DEBUG("Warking APN: %s", apnConfig.toString().c_str());
                contextMap[apnConfig.contextId] = apnConfig;
            }
        }
    }

    std::string Service::prepareQICSGPcmd(const APN::Config &apn)
    {
        ostringstream cmd;
        cmd << "AT+QICSGP=" << static_cast<int>(apn.contextId) << "," << static_cast<int>(apn.type) << ",\"" << apn.apn
            << "\",\"" << apn.username << "\",\"" << apn.password << "\"," << static_cast<int>(apn.authMethod) << "\r";
        return cmd.str();
    }

    sys::Message_t Service::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage * /*resp*/)
    {
        std::shared_ptr<sys::ResponseMessage> responseMsg;

        LOG_DEBUG("ServiceInternet: DataRecieve: bus:%d | message:%d", int(msgl->channel), int(msgl->messageType));

        at::Result retVal;

        switch (static_cast<MessageType>(msgl->messageType)) {
        case MessageType::InternetNotification: {
            NotificationMessage *msg = dynamic_cast<NotificationMessage *>(msgl);
            if (msg) {
                LOG_DEBUG("MessageType::InternetNotification:%s", msg->c_str());
                return std::make_shared<InternetResponseMessage>(true);
            }
        } break;

            //    case MessageType::InternetConfigure: {
            //        LOG_DEBUG("MessageType::InternetConfigure");
            //        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
            //        if (msg)
            //        {
            //            LOG_DEBUG("Ok, configuring");
            //            retVal = dataChannel->cmd( ("AT+QICSGP=1,1,"
            //                                        "\"" + msg->apn + "\","
            //                                        "\"" + msg->user + "\","
            //                                        "\"" + msg->password + "\""
            //                                        "," + static_cast<char>(msg->apnAuthMethod) + "\r").c_str()
            //                                       );
            //            LOG_DEBUG("MessageType::InternetConfigure-set");
            //            if (retVal) {
            //                responseMsg = std::make_shared<InternetResponseMessage>(true);

            //                // Propagate "Data connection configured" notification into system
            //                sys::Bus::SendMulticast(
            //                            std::make_shared<InternetNotificationMessage>(
            //                                //static_cast<InternetNotificationMessage::Type>(InternetNotificationMessage::Type::Configured)),
            //                                InternetNotificationMessage::Type::Configured),
            //                            sys::BusChannels::ServiceInternetNotifications,
            //                            this);
            //                LOG_DEBUG("InternetConfigure OK");
            //            }
            //            else {
            //                responseMsg = std::make_shared<InternetResponseMessage>(false);
            //                LOG_DEBUG("InternetConfigure FAIL");
            //            }
            //        }
            //    } break;

            //    case MessageType::InternetConnect: {
            //        LOG_DEBUG("MessageType::InternetConnect.");
            //        //        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
            //        retVal = dataChannel->cmd("AT+QIACT=1\r", 150000);
            //        if (retVal) {
            //            // Propagate "Data Connecting" notification into system
            //            sys::Bus::SendMulticast(
            //                std::make_shared<NotificationMessage>(
            //                    static_cast<NotificationMessage::Type>(NotificationMessage::Type::Connected)),
            //                sys::BusChannels::ServiceInternetNotifications,
            //                this);
            //            LOG_DEBUG("InternetConnect OK");
            //        }
            //        else {
            //            responseMsg = std::make_shared<InternetResponseMessage>(false);
            //            auto errData = dataChannel->cmd(at::AT::QIGETERROR);
            //            LOG_DEBUG("InternetConnect Fail: %s",
            //                      std::accumulate(errData.response.begin(),
            //                                      errData.response.end(),
            //                                      std::string("")).c_str());
            //        }
            //    } break;

        case MessageType::InternetDisconnect: {
            LOG_DEBUG("MessageType::InternetDisconnect.");
            InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
            retVal                      = dataChannel->cmd(("AT+QIDEACT=1" + msg->data + ";\r").c_str(), 1);
            if (retVal) {
                responseMsg = std::make_shared<InternetResponseMessage>(true);

                // Propagate "Data Connecting" notification into system
                sys::Bus::SendMulticast(std::make_shared<NotificationMessage>(static_cast<NotificationMessage::Type>(
                                            NotificationMessage::Type::Disconnected)),
                                        sys::BusChannels::ServiceInternetNotifications,
                                        this);
                LOG_DEBUG("InternetDisonnect OK");
            }
            else {
                responseMsg = std::make_shared<InternetResponseMessage>(false);
                LOG_DEBUG("InternetDisonnect FAIL");
            }
        } break;

        case MessageType::InternetOpenHTTPConnection: {
            LOG_DEBUG("MessageType::InternetOpenHTTPConnection.");
            //        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
            retVal = dataChannel->cmd("AT+QHTTPCFG=1;\r", 1);
            if (retVal) {
                responseMsg = std::make_shared<InternetResponseMessage>(true);

                // Propagate "Data Connecting" notification into system
                sys::Bus::SendMulticast(std::make_shared<NotificationMessage>(static_cast<NotificationMessage::Type>(
                                            NotificationMessage::Type::RequestProcessed)),
                                        sys::BusChannels::ServiceInternetNotifications,
                                        this);
                LOG_DEBUG("InternetOpenHTTP-contextID OK");
            }
            else {
                responseMsg = std::make_shared<InternetResponseMessage>(false);
                LOG_DEBUG("InternetOpenHTTP-contextID FAILED");
                return responseMsg;
            }

            retVal = dataChannel->cmd("AT+QHTTPCFG=\"requestheader\",1;\r", 1);
            if (retVal) {
                responseMsg = std::make_shared<InternetResponseMessage>(true);

                // Propagate "Data Connecting" notification into system
                sys::Bus::SendMulticast(std::make_shared<NotificationMessage>(static_cast<NotificationMessage::Type>(
                                            NotificationMessage::Type::RequestProcessed)),
                                        sys::BusChannels::ServiceInternetNotifications,
                                        this);
                LOG_DEBUG("InternetOpenHTTP-requestheader OK");
            }
            else {
                responseMsg = std::make_shared<InternetResponseMessage>(false);
                LOG_DEBUG("InternetOpenHTTP-requestheader FAIL");
            }
        } break;

        case MessageType::InternetHTTPUrl: {
            LOG_DEBUG("MessageType::InternetNotificationHTTPUrl.");
            InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
            std::stringstream tmp;
            tmp << "AT+QHTTPURL=" << msg->url.length() << ";\r";

            auto retVal = dataChannel->cmd(tmp.str().c_str(), 10000);
            if (retVal && (retVal.response[0] == "CONNECT")) { // wait for 'COMMAND' from gsm module & send url
                retVal = dataChannel->cmd(msg->url.c_str(), 1);
                LOG_DEBUG("InternetHTTP Url-CONNECT OK");
            }
            if (retVal) {
                responseMsg = std::make_shared<InternetResponseMessage>(true);

                // Propagate "Data Connecting" notification into system
                sys::Bus::SendMulticast(std::make_shared<NotificationMessage>(static_cast<NotificationMessage::Type>(
                                            NotificationMessage::Type::RequestProcessed)),
                                        sys::BusChannels::ServiceInternetNotifications,
                                        this);
                LOG_DEBUG("InternetHTTP Url OK");
            }
            else {
                responseMsg = std::make_shared<InternetResponseMessage>(false);
                LOG_DEBUG("InternetHTTP Url FAIL");
            }
        } break;

        case MessageType::InternetHTTPGET: {
            LOG_DEBUG("InternetNotificationHTTP GET.");
            InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
            std::stringstream tmp;
            tmp << "AT+QHTTPGET=30," << msg->request.length() << ";\r";

            auto retVal = dataChannel->cmd(tmp.str().c_str(), 10000);
            if (retVal) { // wait for 'COMMAND' from gsm module & send request
                retVal = dataChannel->cmd(msg->request.c_str(), 30000);
                LOG_DEBUG("InternetHTTP GET-CONNECT OK");
            }
            if (retVal) {
                responseMsg = std::make_shared<InternetResponseMessage>(true);

                // Propagate "Data Connecting" notification into system
                sys::Bus::SendMulticast(std::make_shared<NotificationMessage>(static_cast<NotificationMessage::Type>(
                                            NotificationMessage::Type::RequestProcessed)),
                                        sys::BusChannels::ServiceInternetNotifications,
                                        this);
                LOG_DEBUG("InternetHTTP GET OK");
            }
            else {
                responseMsg = std::make_shared<InternetResponseMessage>(false);
                LOG_DEBUG("InternetHTTP GET FAIL");
            }
        } break;

        case MessageType::InternetHTTPPOST: {
            //        InternetNotificationMessage *msg = reinterpret_cast<InternetNotificationMessage *>(msgl);
        } break;

        case MessageType::InternetHTTPReadData: {
            LOG_DEBUG("HTTP Read data.");
            //        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);

            auto retVal = dataChannel->cmd("AT+QHTTPREAD=30;\r", 10000);
            if (retVal) { // wait for 'COMMAND' from gsm module
                LOG_DEBUG("HTTP: %s", retVal.response[0].c_str());
            }
            if (retVal) {
                responseMsg = std::make_shared<InternetResponseMessage>(true);

                // Propagate "Data Connecting" notification into system
                sys::Bus::SendMulticast(std::make_shared<NotificationMessage>(static_cast<NotificationMessage::Type>(
                                            NotificationMessage::Type::RequestProcessed)),
                                        sys::BusChannels::ServiceInternetNotifications,
                                        this);
                LOG_DEBUG("InternetHTTP DATA OK");
            }
            else {
                responseMsg = std::make_shared<InternetResponseMessage>(false);
                LOG_DEBUG("InternetHTTP DATA FAIL");
            }
        } break;

        default:
            break;
        }

        return (responseMsg ? responseMsg : std::make_shared<sys::ResponseMessage>());
    }

} // namespace InternetService
