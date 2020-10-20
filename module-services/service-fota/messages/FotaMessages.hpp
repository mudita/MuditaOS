// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-fota/api/FotaServiceAPI.hpp>
#include <Service/Message.hpp>
#include <MessageType.hpp>

#include <memory>
#include <variant>

namespace FotaService
{

    class FotaMessage : public sys::DataMessage
    {
      public:
        FotaMessage(MessageType messageType) : sys::DataMessage(messageType), type(messageType){};

        virtual ~FotaMessage() = default;

        MessageType type;
    };

    class NotificationMessage : public FotaMessage
    {
      public:
        enum class Type
        {
            NotReady,
            Configured,
            Connected,
            Disconnected,
            RequestProcessed,
            ServiceReady, // Idle state of the service. This is a start state before any call is initialized by user or
                          // by network. service returns to this state when call is finished.
        };
        const char *c_str()
        {
            return c_str(type);
        }
        static const char *c_str(Type type)
        {
            switch (type) {
            case NotificationMessage::Type::NotReady:
                return "NotReady";
            case NotificationMessage::Type::Configured:
                return "Configured";
            case NotificationMessage::Type::Connected:
                return "Connected";
            case NotificationMessage::Type::Disconnected:
                return "Disconnected";
            case NotificationMessage::Type::RequestProcessed:
                return "RequestProcessed";
            case NotificationMessage::Type::ServiceReady:
                return "ServiceReady";
            }
            return "";
        }

        NotificationMessage(Type type = Type::NotReady) : FotaMessage(MessageType::FotaInternetNotification), type(type)
        {}

        virtual ~NotificationMessage() = default;

        Type type;
        std::string data;
    };

    class InternetRequestMessage : public FotaMessage
    {
      public:
        enum class Type
        {
            Connect,    // data service connected
            Disconnect, // data service disconnected
            Configure,
        };

        InternetRequestMessage(MessageType messageType) : FotaMessage(messageType)
        {}
        virtual ~InternetRequestMessage() = default;

        std::string data;
        std::string url;
        std::string request;
    };

    class HTTPRequestMessage : public FotaMessage
    {
      public:
        HTTPRequestMessage() : FotaMessage(MessageType::HttpRequest)
        {}

        std::string url; ///< url to download
        HTTPMethod method;
    };

    class HTTPResponseMessage : public FotaMessage
    {
      public:
        HTTPResponseMessage() : FotaMessage(MessageType::HttpResponse)
        {}

        std::string url;                                       ///< requested url (copied from HTTPRequest message)
        HTTPErrors httpError        = HTTPErrors::UnknowError; ///< HTTP commands error
        int httpServerResponseError = 0;                       ///< HTTP response error retruned by server
        std::vector<std::string> responseHeaders;
        std::string body; ///< Content of the file downloade from server
    };

    class FOTAStart : public FotaMessage
    {
      public:
        FOTAStart() : FotaMessage(MessageType::FotaStart)
        {}
        std::string url;
    };

    class FOTAProgres : public FotaMessage
    {
      public:
        FOTAProgres() : FotaMessage(MessageType::FotaProgress)
        {}
        unsigned char progress = 0;
    };

    class FOTARawProgress : public FotaMessage
    {
      public:
        FOTARawProgress() : FotaMessage(MessageType::FotaProgress)
        {}
        std::string qindRaw;
    };

    class FOTAFinished : public FotaMessage
    {
      public:
        FOTAFinished() : FotaMessage(MessageType::FotaFinished)
        {}
    };

    class ConfigureAPNMessage : public FotaMessage
    {
      public:
        ConfigureAPNMessage(const APN::Config &apnConfig = APN::Config())
            : FotaMessage(MessageType::FotaConfigureAPN), apnConfig(apnConfig)
        {}
        virtual ~ConfigureAPNMessage() = default;
        APN::Config apnConfig;
    };

    class ConnectMessage : public FotaMessage
    {
      public:
        ConnectMessage(unsigned char contextId = 1)
            : FotaMessage(MessageType::FotaInternetConnect), contextId(contextId)
        {}
        virtual ~ConnectMessage() = default;
        unsigned char contextId;
    };

    class FotaResponseMessage : public sys::ResponseMessage
    {
      public:
        FotaResponseMessage(uint32_t retCode) : sys::ResponseMessage(), retCode(retCode){};
        virtual ~FotaResponseMessage() = default;

        uint32_t retCode = 0;
        std::vector<std::string> retData;
    };

} // namespace FotaService
