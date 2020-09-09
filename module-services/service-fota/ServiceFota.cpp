#include "api/FotaServiceAPI.hpp"

#include "ServiceFota.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include <module-cellular/at/Result.hpp>
#include <module-cellular/at/URC_QIND.hpp>
#include <module-cellular/Modem/TS0710/TS0710.h>
#include <service-cellular/api/CellularServiceAPI.hpp>

#include "MessageType.hpp"

#include "messages/FotaMessages.hpp"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <sstream>

namespace FotaService
{

    const char *Service::serviceName     = "ServiceFota";
    const TickType_t defaultTimer        = 1000;
    const uint32_t QIDEACTTimeout        = 40000;
    const uint32_t QIACTTimeout          = 150000;
    const char *httpErrorCode200         = "200";
    const uint32_t httpErrorCodeValue200 = 200;

    Service::Service() : sys::Service(serviceName)
    {
        LOG_INFO("[ServiceFota] Initializing");

        busChannels.push_back(sys::BusChannels::ServiceFotaNotifications);
        busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);

        connectionTimer = CreateTimer(defaultTimer, true);
        registerMessageHandlers();
    }

    Service::~Service()
    {
        LOG_INFO("[ServiceFota] Cleaning resources");
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
        LOG_DEBUG("Registring Handlers for Fota::Service:");
        using std::placeholders::_1;
        using std::placeholders::_2;
        connect(CellularGetChannelResponseMessage(),
                std::bind(&Service::handleCellularGetChannelResponseMessage, this, _1, _2));
        connect(cellular::StateChange(), std::bind(&Service::handleServiceCellularNotifications, this, _1, _2));
        connect(ConfigureAPNMessage(), std::bind(&Service::handleConfigureAPN, this, _1, _2));
        connect(ConnectMessage(), std::bind(&Service::handleConnect, this, _1, _2));
        connect(HTTPRequestMessage(), std::bind(&Service::handleHttpGet, this, _1, _2));
        connect(FOTAStart(), std::bind(&Service::handleFotaStart, this, _1, _2));
        connect(FOTARawProgress(), std::bind(&Service::handleRawProgress, this, _1, _2));
    }

    sys::Message_t Service::handleCellularGetChannelResponseMessage(sys::DataMessage *req,
                                                                    sys::ResponseMessage * /*response*/)
    {
        LOG_DEBUG("Handling channel response");
        auto responseMessage = static_cast<CellularGetChannelResponseMessage *>(req);
        LOG_DEBUG("channel ptr: %p", responseMessage->dataChannelPtr);
        dataChannel = responseMessage->dataChannelPtr;
        dataChannel->setCallback(std::bind(&Service::handleChannelNotifications, this, std::placeholders::_1));
        getApnConfiguration();
        setState();
        stopActiveContext();
        return sys::Message_t();
    }

    sys::Message_t Service::handleServiceCellularNotifications(sys::DataMessage *req,
                                                               sys::ResponseMessage * /*response*/)
    {
        if (auto msg = dynamic_cast<cellular::StateChange *>(req)) {
            LOG_DEBUG("cellular::StageChange: %s", cellular::State::c_str(msg->request));
            if (msg->request == cellular::State::ST::Ready) {
                LOG_DEBUG("Modem is \"Ready\"");
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
    }

    sys::Message_t Service::handleConfigureAPN(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
    {
        if (dataChannel != nullptr) {
            if (auto msg = dynamic_cast<ConfigureAPNMessage *>(req)) {
                LOG_DEBUG("Configure APN: %s", msg->apnConfig.toString().c_str());
                auto config =
                    std::find_if(contextMap.begin(), contextMap.end(), [&msg](const APN::ContextPair &pair) -> bool {
                        return pair.second.contextId == msg->apnConfig.contextId;
                    });
                currentApnContext = msg->apnConfig.contextId;

                at::Result atResult;

                if (config == contextMap.end()) {
                    LOG_DEBUG("Configuring new APN: %s", msg->apnConfig.toString().c_str());
                    atResult = sendAndLogError(prepareQICSGPcmd(msg->apnConfig));
                }
                else {
                    if (config->second.contextId == msg->apnConfig.contextId) {
                        if (config->second.activated && (config->second.apn != msg->apnConfig.apn)) {
                            atResult = sendAndLogError(prepareQIDEACT(config->second.contextId), QIDEACTTimeout);
                        }
                        atResult = sendAndLogError(prepareQICSGPcmd(msg->apnConfig));
                    }
                    else {
                        atResult.code = at::Result::Code::OK;
                    }
                }

                if (atResult) {
                    sys::Bus::SendMulticast(
                        std::make_shared<NotificationMessage>(NotificationMessage::Type::Configured),
                        sys::BusChannels::ServiceFotaNotifications,
                        this);
                    LOG_DEBUG("Internet Cofiguration OK");
                }
                return std::make_shared<FotaResponseMessage>(true);
            }
        }
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::Message_t Service::handleConnect(sys::DataMessage * /*req*/, sys::ResponseMessage * /*response*/)
    {
        if (dataChannel) {
            LOG_DEBUG("ConnectMessage");
            getApnConfiguration();
            if (!contextMap[currentApnContext].activated) {
                if (!sendAndLogError(prepareQIACT(currentApnContext), QIACTTimeout)) {
                    return std::make_shared<FotaResponseMessage>(false);
                }
            }
            LOG_DEBUG("InternetConnect OK");
            sys::Bus::SendMulticast(std::make_shared<NotificationMessage>(
                                        static_cast<NotificationMessage::Type>(NotificationMessage::Type::Connected)),
                                    sys::BusChannels::ServiceFotaNotifications,
                                    this);
            return std::make_shared<FotaResponseMessage>(true);
        }
        return std::make_shared<FotaResponseMessage>(false);
    }

    sys::Message_t Service::handleHttpGet(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
    {
        std::shared_ptr<sys::ResponseMessage> responseMsg;
        if (auto msg = dynamic_cast<HTTPRequestMessage *>(req)) {
            normalizeUrl(msg->url);
            url                 = msg->url;
            receiverServiceName = msg->sender;
            LOG_DEBUG("HTTP Get   : '%s' (%d)", msg->url.c_str(), static_cast<int>(msg->url.size()));
            LOG_DEBUG("HTTP Method: %s", (msg->method == HTTPMethod::GET ? "GET" : "POST"));
            // setup http context
            std::vector<std::string> httpGetCommands;
            httpGetCommands.push_back("AT+QHTTPCFG=\"contextid\",1\r");
            httpGetCommands.push_back("AT+QHTTPCFG=\"responseheader\",1\r");
            /// left for debuging - DNS configuration
            // httpGetCommands.push_back("AT+QIDNSCFG=?\r");
            // httpGetCommands.push_back("AT+QIDNSCFG=1\r");
            for (auto &currCmd : httpGetCommands) {
                if (!sendAndLogError(currCmd)) {
                    return std::make_shared<FotaResponseMessage>(false);
                }
            }

            if (isHTTPS(msg->url)) {
                setupSSLContext();
            }
            if (!openURL(msg->url)) {
                return std::make_shared<FotaResponseMessage>(false);
            }

            switch (msg->method) {
            case HTTPMethod::GET: {
                LOG_DEBUG("GET");
                if (!sendAndLogError(prepareQHTTPGET())) {
                    return std::make_shared<FotaResponseMessage>(false);
                }
                break;
            }
            case HTTPMethod::POST:
                LOG_DEBUG("POST - not supported yet");
                break;
            }
            return std::make_shared<FotaResponseMessage>(true);
        }
        return std::make_shared<FotaResponseMessage>(false);
    }

    void Service::handleChannelNotifications(std::vector<uint8_t> &data)
    {
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

            if (tags[1] == httpErrorCode200) {
                LOG_DEBUG("HTTP Response 200 - reading data");
                // potentialy critical logs, will be removed with FOTA Window commits
                // int timeout = 10;
                // dataChannel->cmd("AT+QHTTPREAD=" + std::to_string(timeout) + "\r" /*, timeout * 1000*/);
                state = State::QHTTPREAD;
                file.clear();
                dataChannel->cmd(prepareQHTTPREAD());
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
    }

    sys::Message_t Service::handleFotaStart(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
    {
        LOG_DEBUG("handle Fota Start");
        if (auto msg = dynamic_cast<FOTAStart *>(req)) {
            LOG_DEBUG("Starting fota update: %s", msg->url.c_str());
            receiverServiceName = msg->sender;
            state               = State::FOTAUpdate;
            auto result         = dataChannel->cmd(prepareQFOTADLcmd(msg->url));
            if (!result) {
                LOG_WARN("Starting fota error: %s",
                         std::accumulate(result.response.begin(), result.response.end(), std::string("\n")).c_str());
            }
        }
        return std::make_shared<FotaResponseMessage>(true);
    }

    sys::Message_t Service::handleRawProgress(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
    {
        LOG_DEBUG("Handle Fota RawProgress message");
        if (auto msg = dynamic_cast<FOTARawProgress *>(req)) {
            parseQIND(msg->qindRaw);
        }
        return std::make_shared<FotaResponseMessage>(true);
    }

    void Service::getApnConfiguration()
    {
        getActiveCotext();
        getConfig();
        for (auto &[contextId, apn] : contextMap) {
            LOG_DEBUG("%d: %s", contextId, apn.toString().c_str());
        }
    }

    void Service::getConfig()
    {
        if (dataChannel) {
            for (auto &[contextId, apn] : contextMap) {
                if (auto data = sendAndLogError(prepareQICSGPquery(apn))) {
                    const std::string QICSGP_prefix("+QICSGP:");
                    for (auto &line : data.response) {
                        if (line.find(QICSGP_prefix) != std::string::npos) {
                            istringstream raw_data(line.substr(QICSGP_prefix.size()));
                            std::vector<std::string> data;
                            std::string subitem;
                            while (std::getline(raw_data, subitem, ',')) {
                                LOG_DEBUG("   %s: '%s'", QICSGP_prefix.c_str(), subitem.c_str());
                                data.push_back(subitem);
                            }
                            try {
                                APN::ContextType configuredType = static_cast<APN::ContextType>(std::stoi(data[0]));
                                if (apn.type != configuredType) {
                                    LOG_INFO("Different content type active vs configured-> %d vs %d",
                                             static_cast<int>(apn.type),
                                             static_cast<int>(configuredType));
                                }
                                apn.apn        = data[1].substr(1, data[1].size() - 2);
                                apn.username   = data[2].substr(1, data[2].size() - 2);
                                apn.password   = data[3].substr(1, data[3].size() - 2);
                                apn.authMethod = static_cast<APN::AuthMethod>(std::stoi(data[4]));
                            }
                            catch (...) {
                                LOG_DEBUG("Throw during cofniguration parsing.");
                            }
                        }
                    }
                }
            }
        }
    }

    void Service::getActiveCotext()
    {
        if (dataChannel) {
            auto availableContext = sendAndLogError("AT+QIACT?\r");
            if (availableContext) {
                parseQIACT(availableContext);
            }
        }
    }

    void Service::stopActiveContext()
    {
        for (auto &context : contextMap) {
            if (context.second.activated) {
                sendAndLogError(prepareQIDEACT(context.second.contextId));
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
                try {
                    apnConfig.contextId = static_cast<unsigned char>(std::stoi(data[0]));
                }
                catch (...) {
                    LOG_DEBUG("Wront context id: %s - setting default: 1", data[0].c_str());
                    apnConfig.contextId = 1;
                }
                apnConfig.activated = (data[1] == "1" ? true : false);
                int contextTypeRaw  = 0;
                try {
                    contextTypeRaw = std::stoi(data[2]);
                }
                catch (const std::exception &e) {
                    LOG_ERROR("Conversion error of %s, taking default value %d", data[2].c_str(), contextTypeRaw);
                }

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

    string Service::prepareQIACT(unsigned char contextId)
    {
        ostringstream cmd;
        cmd << "AT+QIACT=" << static_cast<int>(contextId) << "\r";
        return cmd.str();
    }

    std::string Service::prepareQICSGPcmd(const APN::Config &apn)
    {
        ostringstream cmd;
        cmd << "AT+QICSGP=" << static_cast<int>(apn.contextId) << "," << static_cast<int>(apn.type) << ",\"" << apn.apn
            << "\",\"" << apn.username << "\",\"" << apn.password << "\"," << static_cast<int>(apn.authMethod) << "\r";
        return cmd.str();
    }

    string Service::prepareQICSGPquery(const APN::Config &apn)
    {
        ostringstream cmd;
        cmd << "AT+QICSGP=" << static_cast<int>(apn.contextId) << "\r";
        return cmd.str();
    }

    string Service::prepareQIDEACT(unsigned char contextId)
    {
        ostringstream cmd;
        cmd << "AT+QIDEACT=" << static_cast<int>(contextId) << "\r";
        return cmd.str();
    }

    string Service::prepareQFOTADLcmd(const string &url)
    {
        std::string fotaCmd("AT+QFOTADL=\"");
        fotaCmd += url;
        fotaCmd += "\"\r";
        return fotaCmd;
    }

    string Service::prepareQHTTPGET(unsigned int timeout)
    {
        ostringstream cmd;
        cmd << "AT+QHTTPGET=" << timeout << "\r";
        return cmd.str();
    }

    string Service::prepareQHTTPREAD(unsigned int timeout)
    {
        ostringstream cmd;
        cmd << "AT+QHTTPREAD=" << timeout << "\r";
        return cmd.str();
    }

    string Service::prepareQHTTPURL(const string &url)
    {
        ostringstream cmd;
        cmd << "AT+QHTTPURL=" << url.size() << "\r";
        return cmd.str();
    }

    void Service::setupSSLContext()
    {
        std::vector<std::string> sslConfigCommands;
        sslConfigCommands.push_back("AT+QHTTPCFG=\"sslctxid\",1\r");
        sslConfigCommands.push_back("AT+QSSLCFG=\"sslversion\",1,5\r");
        sslConfigCommands.push_back("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF\r");
        sslConfigCommands.push_back("AT+QSSLCFG=\"seclevel\",1,0\r");

        for (auto &currCmd : sslConfigCommands) {
            sendAndLogError(currCmd);
        }
    }

    bool Service::openURL(const string &url)
    {
        auto response = dataChannel->cmd(prepareQHTTPURL(url));
        if (response.response[0] == "CONNECT") {
            response = dataChannel->cmd(url.c_str());
            logIfError(response, url);
            if (!response) {
                return false;
            }
        }
        else {
            return false;
        }
        return true;
    }

    at::Result Service::sendAndLogError(const std::string &msg) const
    {
        at::Result result = dataChannel->cmd(msg);
        logIfError(result, msg);
        return result;
    }

    at::Result Service::sendAndLogError(const string &msg, uint32_t timeout) const
    {
        at::Result result = dataChannel->cmd(msg, timeout);
        logIfError(result, msg);
        return result;
    }
    void Service::logIfError(const at::Result &result, const std::string &cmdString) const
    {
        if (!result) {
            auto results = dataChannel->cmd(at::AT::QIGETERROR);
            LOG_WARN("error cmd:%s", cmdString.c_str());
            LOG_WARN("Error str:%s",
                     std::accumulate(results.response.begin(), results.response.end(), std::string("\n")).c_str());
        }
    }

    void Service::parseResponse()
    {
        LOG_DEBUG("Got full fille: \n%s", file.c_str());
        std::istringstream input(file);
        enum class HTTPResponsePart
        {
            URC,
            Headers,
            Body
        } part;
        part = HTTPResponsePart::URC;
        std::vector<string> headers;
        std::string body;
        std::string line;
        while (std::getline(input, line, '\n')) {
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

            LOG_DEBUG("line -> '%s'", line.c_str());
            switch (part) {
            case HTTPResponsePart::URC:
                if (line.find("CONNECT") != std::string::npos) {
                    part = HTTPResponsePart::Headers;
                }
                continue;
            case HTTPResponsePart::Headers:
                if (line.empty()) {
                    part = HTTPResponsePart::Body;
                    LOG_DEBUG("=== body part! ===");
                }
                else {
                    headers.push_back(line);
                }
                break;
            case HTTPResponsePart::Body:
                body += line + '\n';
                break;
            }
        }
        std::shared_ptr<HTTPResponseMessage> msg = std::make_shared<HTTPResponseMessage>();
        msg->url                                 = url;
        msg->httpError                           = HTTPErrors::OK;
        msg->httpServerResponseError             = httpErrorCodeValue200;
        msg->responseHeaders                     = std::move(headers);
        msg->body                                = std::move(body);
        sys::Bus::SendUnicast(msg, receiverServiceName, this);
    }

    void Service::parseQIND(const string &message)
    {
        auto qind = at::urc::QIND(message);
        if (qind.is()) {
            const unsigned char fotaPrefixTagPosition = 0;
            const unsigned char fotaStatusTagPosition = 1;
            if (qind.tokens[fotaPrefixTagPosition].find("FOTA") != std::string::npos) {
                if (qind.tokens[1].find("START") != std::string::npos) {
                    LOG_DEBUG("FOTA UPDATING");
                }
                else if (qind.tokens[fotaStatusTagPosition].find("HTTPEND") != std::string::npos) {
                    LOG_DEBUG("Downloading finished: %s", qind.tokens[2].c_str());
                }
                else if (qind.tokens[fotaStatusTagPosition].find("END") != std::string::npos) {
                    LOG_DEBUG("FOTA FINISHED -> reboot (%s)", receiverServiceName.c_str());
                    sendFotaFinshed(receiverServiceName);
                }
                else if (qind.tokens[fotaStatusTagPosition].find("UPDATING") != std::string::npos) {
                    auto token_val = 0;
                    try {
                        token_val = std::stoi(qind.tokens[2]);
                    }
                    catch (const std::exception &e) {
                        LOG_ERROR("Conversion error of %s, taking default value %d", qind.tokens[2].c_str(), token_val);
                    }

                    unsigned char progress = static_cast<unsigned char>(token_val);
                    LOG_DEBUG("FOTA UPDATING: %d", progress);
                    sendProgress(progress, receiverServiceName);
                }
                else if (qind.tokens[fotaStatusTagPosition].find("HTTPSTART") != std::string::npos) {
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

        LOG_DEBUG("%s: DataRecieve: bus:%d | message:%d",
                  serviceName,
                  static_cast<int>(msgl->channel),
                  static_cast<int>(msgl->messageType));
        return (responseMsg ? responseMsg : std::make_shared<sys::ResponseMessage>());
    }

} // namespace FotaService
