#pragma once

#include <service-internet/api/InternetServiceAPI.hpp>
#include <Service/Message.hpp>
#include <MessageType.hpp>

#include <memory>
#include <variant>

namespace InternetService
{

    class InternetMessage : public sys::DataMessage
    {
      public:
        InternetMessage(MessageType messageType) : sys::DataMessage(messageType), type(messageType){};

        virtual ~InternetMessage(){};

        MessageType type;
    };

    class NotificationMessage : public InternetMessage
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

        NotificationMessage(Type type = Type::NotReady) : InternetMessage(MessageType::InternetNotification), type(type)
        {}

        ~NotificationMessage()
        {}

        Type type;
        std::string data;
    };

    class InternetRequestMessage : public InternetMessage
    {
      public:
        enum class Type
        {
            Connect,    // data service connected
            Disconnect, // data service disconnected
            Configure,
        };

        InternetRequestMessage(MessageType messageType) : InternetMessage(messageType)
        {}
        ~InternetRequestMessage()
        {}

        std::string data;
        std::string url;
        std::string request;
    };

    class HTTPRequestMessage : public InternetMessage
    {
      public:
        HTTPRequestMessage() : InternetMessage(MessageType::HttpRequest)
        {}

        std::string url; ///< url to download
        HTTPMethod method;
    };

    class HTTPResponseMessage : public InternetMessage
    {
      public:
        HTTPResponseMessage() : InternetMessage(MessageType::HttpResponse)
        {}

        std::string url;             ///< requested url (copied from HTTPRequest message)
        HTTPErrors httpError;        ///< HTTP commands error
        int httpServerResponseError; ///< HTTP response error retruned by server
        std::vector<std::string> responseHeaders;
        std::string body; ///< Content of the file downloade from server
    };

    class FOTAStart : public InternetMessage
    {
      public:
        FOTAStart() : InternetMessage(MessageType::FotaStart)
        {}
        std::string url;
    };

    class FOTAProgres : public InternetMessage
    {
      public:
        FOTAProgres() : InternetMessage(MessageType::FotaProgress)
        {}
        unsigned char progress = 0;
    };

    class FOTAFinished : public InternetMessage
    {
      public:
        FOTAFinished() : InternetMessage(MessageType::FotaFinished)
        {}
    };

    class ConfigureAPNMessage : public InternetMessage
    {
      public:
        ConfigureAPNMessage(const APN::Config &apnConfig = APN::Config())
            : InternetMessage(MessageType::InternetConfigureAPN), apnConfig(apnConfig)
        {}
        ~ConfigureAPNMessage() = default;
        APN::Config apnConfig;
    };

    class ConnectMessage : public InternetMessage
    {
      public:
        ConnectMessage(unsigned char contextId = 1)
            : InternetMessage(MessageType::InternetConnect), contextId(contextId)
        {}
        ~ConnectMessage() = default;
        unsigned char contextId;
    };

    class InternetResponseMessage : public sys::ResponseMessage
    {
      public:
        InternetResponseMessage(uint32_t retCode) : sys::ResponseMessage(), retCode(retCode){};
        virtual ~InternetResponseMessage(){};

        uint32_t retCode;
        std::vector<std::string> retData;
    };

} // namespace InternetService
