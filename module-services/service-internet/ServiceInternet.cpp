/*
 *  @file ServiceInternet.cpp
 *  @author Łukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 *  @date 13.09.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "ServiceInternet.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include "Modem/MuxDaemon.hpp"

#include "MessageType.hpp"

#include "messages/InternetMessage.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include "service-Internet/api/InternetServiceAPI.hpp"

const char *ServiceInternet::serviceName = "ServiceInternet";

ServiceInternet::ServiceInternet() : sys::Service(serviceName)
{
    LOG_INFO("[ServiceInternet] Initializing");
    cmux = std::make_unique(PortSpeed_e::PS460800, this);

    busChannels.push_back(sys::BusChannels::ServiceInternetNotifications);

    connectionTimer = CreateTimer(1000, true);
    /*
        notificationCallback = [this](NotificationType type, std::string resp) {

            auto msg = std::make_shared<InternetNotificationMessage>(static_cast<InternetNotificationMessage::Type
       >(type));

            switch (type) {

                case NotificationType::DataTransferConnected:
                    break;

                case NotificationType::DataTransferDisconnected:
                    break;
            }


            sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceInternetNotifications, this);

        };
    */
}

ServiceInternet::~ServiceInternet()
{
    LOG_INFO("[ServiceInternet] Cleaning resources");
}

// Invoked when timer ticked
void ServiceInternet::TickHandler(uint32_t id)
{
    std::shared_ptr<InternetRequestMessage> msg =
        std::make_shared<InternetRequestMessage>(MessageType::CellularListCurrentCalls);

    sys::Bus::SendUnicast(msg, ServiceInternet::serviceName, this);
}

// Invoked during initialization
sys::ReturnCodes ServiceInternet::InitHandler()
{

    //    muxdaemon = MuxDaemon::Create(notificationCallback);
    //    if (muxdaemon) {

    //        state = State::Idle;

    return sys::ReturnCodes::Success;
    //    }
    //    else {
    //        return sys::ReturnCodes::Failure;
    //    }
}

sys::ReturnCodes ServiceInternet::DeinitHandler()
{

    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceInternet::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<sys::ResponseMessage> responseMsg;

    LOG_DEBUG("ServiceInternet: DataRecieve.");

    auto channel = cmux->get(TS0710::Channel::Data);
    bool retVal(false);

    switch (static_cast<MessageType>(msgl->messageType)) {
    // Incoming notifications from Notification Virtual Channel
    case MessageType::InternetNotification: {
        LOG_DEBUG("MessageType::InternetNotification")
        InternetNotificationMessage *msg = reinterpret_cast<InternetNotificationMessage *>(msgl);

        // if (msg->type == InternetNotificationMessage::Type::Disconnect) {
        //     stopTimer(connectionTimer);
        // } else if (msg->type == InternetNotificationMessage::Type::Connect) {
        //     // activate call state timer
        //     ReloadTimer(connectionTimer);
        // }
        // else {

        // }
    } break;

    case MessageType::InternetConfigure: {
        LOG_DEBUG("MessageType::InternetConfigure");
        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
        retVal                      = channel->cmd(("AT+QICSGP=1,1,"
                               "\"" +
                               msg->apn +
                               "\","
                               "\"" +
                               msg->user +
                               "\","
                               "\"" +
                               msg->password +
                               "\","
                               "1" +
                               ";\r")
                                  .c_str(),
                              1);
        if ((retVal.size() == 1) && (retVal[0] == "OK")) {
            responseMsg = std::make_shared<InternetResponseMessage>(true);

            // Propagate "Data connection configured" notification into system
            sys::Bus::SendMulticast(
                std::make_shared<InternetNotificationMessage>(
                    static_cast<InternetNotificationMessage::Type>(InternetNotificationMessage::Type::Configured)),
                sys::BusChannels::ServiceInternetNotifications,
                this);
            LOG_DEBUG("InternetConfigure OK");
        }
        else {
            responseMsg = std::make_shared<InternetResponseMessage>(false);
            LOG_DEBUG("InternetConfigure FAIL");
        }
    } break;

    case MessageType::InternetConnect: {
        LOG_DEBUG("MessageType::InternetConnect.");
        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
        retVal                      = channel->cmd("AT+QIACT=1;\r", 1);
        if ((retVal.size() == 1) && (retVal[0] == "OK")) {
            // Propagate "Data Connecting" notification into system
            sys::Bus::SendMulticast(
                std::make_shared<InternetNotificationMessage>(
                    static_cast<InternetNotificationMessage::Type>(InternetNotificationMessage::Type::Connected)),
                sys::BusChannels::ServiceInternetNotifications,
                this);
            LOG_DEBUG("InternetConnect OK");
        }
        else {
            responseMsg = std::make_shared<InternetResponseMessage>(false);
            LOG_DEBUG("InternetConnect FAIL");
        }
    } break;

    case MessageType::InternetDisconnect: {
        LOG_DEBUG("MessageType::InternetDisconnect.");
        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
        retVal                      = channel->cmd(("AT+QIDEACT=1" + msg->data + ";\r").c_str(), 1);
        if ((retVal.size() == 1) && (retVal[0] == "OK")) {
            responseMsg = std::make_shared<InternetResponseMessage>(true);

            // Propagate "Data Connecting" notification into system
            sys::Bus::SendMulticast(
                std::make_shared<InternetNotificationMessage>(
                    static_cast<InternetNotificationMessage::Type>(InternetNotificationMessage::Type::Disconnected)),
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
        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);
        auto retVal                 = channel->cmd("AT+QHTTPCFG=1;\r", 1);
        if ((retVal.size() == 1) && (retVal[0] == "OK")) {
            responseMsg = std::make_shared<InternetResponseMessage>(true);

            // Propagate "Data Connecting" notification into system
            sys::Bus::SendMulticast(
                std::make_shared<InternetNotificationMessage>(static_cast<InternetNotificationMessage::Type>(
                    InternetNotificationMessage::Type::RequestProcessed)),
                sys::BusChannels::ServiceInternetNotifications,
                this);
            LOG_DEBUG("InternetOpenHTTP-contextID OK");
        }
        else {
            responseMsg = std::make_shared<InternetResponseMessage>(false);
            LOG_DEBUG("InternetOpenHTTP-contextID FAILED");
            return responseMsg;
        }

        retVal = channel->cmd("AT+QHTTPCFG=\"requestheader\",1;\r", 1);
        if ((retVal.size() == 1) && (retVal[0] == "OK")) {
            responseMsg = std::make_shared<InternetResponseMessage>(true);

            // Propagate "Data Connecting" notification into system
            sys::Bus::SendMulticast(
                std::make_shared<InternetNotificationMessage>(static_cast<InternetNotificationMessage::Type>(
                    InternetNotificationMessage::Type::RequestProcessed)),
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

        auto retVal = channel->cmd(tmp.str().c_str(), 10000);
        if ((retVal.size() == 1) && (retVal[0] == "CONNECT")) { // wait for 'COMMAND' from gsm module & send url
            retVal = channel->cmd(msg->url.c_str(), 1);
            LOG_DEBUG("InternetHTTP Url-CONNECT OK");
        }
        if ((retVal.size() == 1) && (retVal[0] == "OK")) {
            responseMsg = std::make_shared<InternetResponseMessage>(true);

            // Propagate "Data Connecting" notification into system
            sys::Bus::SendMulticast(
                std::make_shared<InternetNotificationMessage>(static_cast<InternetNotificationMessage::Type>(
                    InternetNotificationMessage::Type::RequestProcessed)),
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

        auto retVal = channel->cmd(tmp.str().c_str(), 10000);
        if ((retVal.size() == 1) && (retVal[0] == "CONNECT")) { // wait for 'COMMAND' from gsm module & send request
            retVal = channel->cmd(msg->request.c_str(), 30000);
            LOG_DEBUG("InternetHTTP GET-CONNECT OK");
        }
        if ((retVal.size() == 1) && (retVal[0] == "OK")) {
            responseMsg = std::make_shared<InternetResponseMessage>(true);

            // Propagate "Data Connecting" notification into system
            sys::Bus::SendMulticast(
                std::make_shared<InternetNotificationMessage>(static_cast<InternetNotificationMessage::Type>(
                    InternetNotificationMessage::Type::RequestProcessed)),
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
        InternetNotificationMessage *msg = reinterpret_cast<InternetNotificationMessage *>(msgl);
    } break;

    case MessageType::InternetHTTPReadData: {
        LOG_DEBUG("HTTP Read data.");
        InternetRequestMessage *msg = reinterpret_cast<InternetRequestMessage *>(msgl);

        auto retVal = channel->cmd("AT+QHTTPREAD=30;\r", 10000);
        if (retVal[0] == "CONNECT") { // wait for 'COMMAND' from gsm module
            LOG_DEBUG("HTTP: %s", retVal[1].c_str());
        }
        if (retVal.back() == "OK") {
            responseMsg = std::make_shared<InternetResponseMessage>(true);

            // Propagate "Data Connecting" notification into system
            sys::Bus::SendMulticast(
                std::make_shared<InternetNotificationMessage>(static_cast<InternetNotificationMessage::Type>(
                    InternetNotificationMessage::Type::RequestProcessed)),
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

    return responseMsg;
}
