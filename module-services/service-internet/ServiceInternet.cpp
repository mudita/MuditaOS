#include "api/InternetServiceAPI.hpp"

#include "ServiceInternet.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include <module-cellular/at/Result.hpp>
#include <module-cellular/at/URC_QIND.hpp>
#include <module-cellular/Modem/TS0710/TS0710.h>
#include <service-cellular/api/CellularServiceAPI.hpp>

#include "MessageType.hpp"

#include "messages/InternetMessage.hpp"

#include <algorithm>
#include <cctype>
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
        handleHttpGet();
        handleFotaStart();
    }

    void Service::handleCellularGetChannelResponseMessage()
    {
        LOG_DEBUG("  -CellularGetChannelResponseMessage");
        connect(CellularGetChannelResponseMessage(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
            LOG_DEBUG("Handling channel response");
            auto responseMessage = static_cast<CellularGetChannelResponseMessage *>(req);
            LOG_DEBUG("chanel ptr: %p", responseMessage->dataChannelPtr);
            dataChannel = responseMessage->dataChannelPtr;
            handleNotifications();
            getApnConfiguration();
            setState();
            stopActiveContext();
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

    void Service::handleHttpGet()
    {
        connect(
            HTTPRequestMessage(),
            [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) -> std::shared_ptr<sys::ResponseMessage> {
                std::shared_ptr<sys::ResponseMessage> responseMsg;
                if (auto msg = dynamic_cast<HTTPRequestMessage *>(req)) {
                    normalizeUrl(msg->url);
                    url                 = msg->url;
                    receiverServiceName = msg->sender;
                    LOG_DEBUG("HTTP Get   : '%s' (%d)", msg->url.c_str(), (int)msg->url.size());
                    LOG_DEBUG("HTTP Method: %s", (msg->method == HTTPMethod::GET ? "GET" : "POST"));
                    // setup http context
                    std::string currCmd;
                    currCmd = "AT+QHTTPCFG=\"contextid\",1\r";
                    if (!dataChannel->cmd(currCmd)) {
                        messageError(currCmd);
                        return std::make_shared<InternetResponseMessage>(false);
                    }
                    currCmd = "AT+QHTTPCFG=\"responseheader\",1\r";
                    if (!dataChannel->cmd(currCmd)) {
                        messageError(currCmd);
                        return std::make_shared<InternetResponseMessage>(false);
                    }
                    currCmd = "AT+QIDNSCFG=?\r";
                    if (!dataChannel->cmd(currCmd)) {
                        messageError(currCmd);
                        return std::make_shared<InternetResponseMessage>(false);
                    }
                    currCmd = "AT+QIDNSCFG=1\r";
                    if (!dataChannel->cmd(currCmd)) {
                        messageError(currCmd);
                        return std::make_shared<InternetResponseMessage>(false);
                    }

                    if (isHTTPS(msg->url)) {
                        setupSSLContext();
                    }
                    if (!openURL(msg->url)) {
                        return std::make_shared<InternetResponseMessage>(false);
                    }

                    switch (msg->method) {
                    case HTTPMethod::GET: {
                        LOG_DEBUG("GET");
                        currCmd = "AT+QHTTPGET=20\r";
                        if (!dataChannel->cmd(currCmd)) {
                            auto response = dataChannel->cmd(at::AT::QIGETERROR);
                            LOG_DEBUG(
                                "error: %s",
                                std::accumulate(response.response.begin(), response.response.end(), std::string("\n\t"))
                                    .c_str());
                            return std::make_shared<InternetResponseMessage>(false);
                        }
                        break;
                    }
                    case HTTPMethod::POST:
                        LOG_DEBUG("POST - not supported yet");
                        break;
                    }
                    LOG_DEBUG("=== === ===");
                    return std::make_shared<InternetResponseMessage>(true);
                }
                return std::make_shared<InternetResponseMessage>(false);
            });
    }

    void Service::handleNotifications()
    {
        dataChannel->setCallback([&](std::vector<uint8_t> &data) -> void {
            const std::string QHTTPGET("+QHTTPGET:");
            std::vector<uint8_t> buffer(data.begin(), data.end());
            TS0710_Frame::frame_t frame;
            frame.deserialize(buffer);

            std::string response(frame.data.begin(), frame.data.end());
            LOG_DEBUG("URC: %s", response.c_str());

            if (auto begin = response.find(QHTTPGET) != std::string::npos) {
                auto end = response.find("\r", begin);
                unsigned int dataPosition;
                dataPosition = begin + QHTTPGET.size();
                std::istringstream msg(
                    response.substr(dataPosition, (end != std::string::npos ? end : response.size()) - dataPosition));
                std::string tag;
                std::vector<std::string> tags;
                while (std::getline(msg, tag, ',')) {
                    tags.push_back(tag);
                }

                if (tags[1] == "200") {
                    LOG_DEBUG("HTTP Response 200 - reading data");
                    int timeout = 10;
                    dataChannel->cmd("AT+QHTTPREAD=" + std::to_string(timeout) + "\r" /*, timeout * 1000*/);
                    state = State::QHTTPREAD;
                    file.clear();
                }
            }

            if (state == State::QHTTPREAD) {
                LOG_DEBUG("QHTTPREAD");
                if (response.find("+QHTTPREAD: 0") != std::string::npos) {
                    LOG_DEBUG("QHTTPREAD:finished");
                    state = State::Connected;
                    parseResponse();
                }
                else {
                    LOG_DEBUG("QHTTPREAD: append: |%s|", response.c_str());
                    file += response;
                }
            }

            parseQIND(response);
        });
    }

    void Service::handleFotaStart()
    {
        connect(
            FOTAStart(),
            [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) -> std::shared_ptr<sys::ResponseMessage> {
                LOG_DEBUG("handle Fota Start");
                if (auto msg = dynamic_cast<FOTAStart *>(req)) {
                    LOG_DEBUG("Starting fota update: %s", msg->url.c_str());
                    receiverServiceName = msg->sender;
                    state               = State::FOTAUpdate;
                    LOG_DEBUG("fota cmd: %s", prepareQFOTADLcmd(msg->url).c_str());
                    dataChannel->cmd(prepareQFOTADLcmd(msg->url));
                }
                return std::make_shared<InternetResponseMessage>(true);
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

    void Service::stopActiveContext()
    {
        std::string deactivateCmd = "AT+QIDEACT=";
        for (auto &context : contextMap) {
            if (context.second.activated) {
                dataChannel->cmd(deactivateCmd + std::to_string(context.second.contextId) + "\r");
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

    bool Service::isHTTPS(const std::string &url) const
    {
        return url.find("https://") != std::string::npos;
    }

    void Service::normalizeUrl(std::string &url) const
    {
        std::transform(url.begin(), url.end(), url.begin(), [](unsigned char chr) { return std::tolower(chr); });
    }

    std::string Service::prepareQICSGPcmd(const APN::Config &apn)
    {
        ostringstream cmd;
        cmd << "AT+QICSGP=" << static_cast<int>(apn.contextId) << "," << static_cast<int>(apn.type) << ",\"" << apn.apn
            << "\",\"" << apn.username << "\",\"" << apn.password << "\"," << static_cast<int>(apn.authMethod) << "\r";
        return cmd.str();
    }

    string Service::prepareQFOTADLcmd(const string &url)
    {
        std::string fotaCmd("AT+QFOTADL=\"");
        fotaCmd += url;
        fotaCmd += "\"\r";
        return fotaCmd;
    }

    void Service::setupSSLContext()
    {
        std::vector<std::string> sslConfigCommands;
        sslConfigCommands.push_back("AT+QHTTPCFG=\"sslctxid\",1\r");
        sslConfigCommands.push_back("AT+QSSLCFG=\"sslversion\",1,5\r");
        sslConfigCommands.push_back("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF\r");
        sslConfigCommands.push_back("AT+QSSLCFG=\"seclevel\",1,0\r");

        for (auto &currCmd : sslConfigCommands) {
            auto response = dataChannel->cmd(currCmd);
            if (!response) {
                response = dataChannel->cmd(at::AT::QIGETERROR);
                LOG_WARN(
                    "error cmd: %s:%s",
                    currCmd.c_str(),
                    std::accumulate(response.response.begin(), response.response.end(), std::string("\n\t")).c_str());
            }
        }
    }

    bool Service::openURL(const string &url)
    {
        std::stringstream currCmnd;
        //        int timeInSec(60);
        //        currCmnd << "AT+QHTTPURL=" << url.size() << "," << timeInSec << "\r";
        currCmnd << "AT+QHTTPURL=" << url.size() << "\r";
        auto response = dataChannel->cmd(currCmnd.str().c_str());
        if (/*response &&*/ (response.response[0] == "CONNECT")) {
            response = dataChannel->cmd(url.c_str());
            if (!response) {
                messageError("open url error");
                return false;
            }
        }
        else {
            LOG_INFO("open url error: %s",
                     std::accumulate(response.response.begin(), response.response.end(), std::string("\n")).c_str());
            return false;
        }
        return true;
    }

    void Service::messageError(const std::string &msg) const
    {
        auto results = dataChannel->cmd(at::AT::QIGETERROR);
        LOG_WARN("%s:%s",
                 msg.c_str(),
                 std::accumulate(results.response.begin(), results.response.end(), std::string("\n")).c_str());
        // send messatge
    }

    void Service::parseResponse()
    {
        LOG_DEBUG("Got full fille: \n%s", file.c_str());
        std::istringstream input(file);
        enum Part
        {
            URC,
            Headers,
            Body
        } part;
        part = URC;
        std::vector<string> headers;
        std::string body;
        std::string line;
        while (std::getline(input, line, '\n')) {
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

            LOG_DEBUG("line -> '%s'", line.c_str());
            switch (part) {
            case URC:
                if (line.find("CONNECT") != std::string::npos) {
                    part = Headers;
                }
                continue;
            case Headers:
                if (line.empty()) {
                    part = Body;
                    LOG_DEBUG("=== body part! ===");
                }
                else {
                    headers.push_back(line);
                }
                break;
            case Body:
                body += line + '\n';
                break;
            }
        }
        std::shared_ptr<HTTPResponseMessage> msg = std::make_shared<HTTPResponseMessage>();
        msg->url                                 = url;
        msg->httpError                           = HTTPErrors::OK;
        msg->httpServerResponseError             = 200;
        msg->responseHeaders                     = std::move(headers);
        msg->body                                = std::move(body);
        sys::Bus::SendUnicast(msg, receiverServiceName, this);
    }

    void Service::parseQIND(const string &message)
    {
        auto qind = at::urc::QIND(message);
        if (qind.is()) {
            if (qind.tokens[0].find("FOTA") != std::string::npos) {
                if (qind.tokens[1].find("START") != std::string::npos) {
                    LOG_DEBUG("FOTA UPDATING");
                }
                else if (qind.tokens[1].find("HTTPEND") != std::string::npos) {
                    LOG_DEBUG("Downloading finished: %s", qind.tokens[2].c_str());
                }
                else if (qind.tokens[1].find("END") != std::string::npos) {
                    LOG_DEBUG("FOTA FINISHED -> reboot");
                    sendFotaFinshed(receiverServiceName);
                }
                else if (qind.tokens[1].find("UPDATING") != std::string::npos) {
                    unsigned char progress = static_cast<unsigned char>(std::stoi(qind.tokens[2]));
                    LOG_DEBUG("FOTA UPDATING: %d", progress);
                    sendProgress(progress, receiverServiceName);
                }
                else if (qind.tokens[1].find("HTTPSTART") != std::string::npos) {
                    LOG_DEBUG("Start downloading DELTA");
                }
            }
        }
    }

    void Service::sendProgress(unsigned int progress, const string &receiver)
    {
        auto progressMsg      = std::make_shared<FOTAProgres>();
        progressMsg->progress = progress;
        sys::Bus::SendUnicast(progressMsg, receiver, this);
    }

    void Service::sendFotaFinshed(const string &receiver)
    {
        auto msg = make_shared<FOTAFinished>();
        sys::Bus::SendUnicast(std::move(msg), receiver, this);
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
            retVal = dataChannel->cmd("AT+QHTTPCFG=?;\r", 1);
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

            auto retVal = dataChannel->cmd("AT+QHTTPREAD=30;\r", 30000);
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
