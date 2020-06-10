#include "FotaServiceAPI.hpp"

#include <service-fota/ServiceFota.hpp>
#include <service-fota/messages/FotaMessages.hpp>
#include <service-cellular/messages/CellularMessage.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <Service/Bus.hpp>

#include <sstream>
#include <string>

namespace FotaService
{

    bool API::Configure(sys::Service *serv, const APN::Config &config)
    {
        LOG_DEBUG("FOTA - Internet Config called");
        std::shared_ptr<ConfigureAPNMessage> msg = std::make_shared<ConfigureAPNMessage>(config);
        return sys::Bus::SendUnicast(std::move(msg), FotaService::Service::serviceName, serv);
    }

    bool API::Connect(sys::Service *serv)
    {
        LOG_DEBUG("FOTA - Internet connection called");
        auto msg = std::make_shared<ConnectMessage>();
        return sys::Bus::SendUnicast(std::move(msg), FotaService::Service::serviceName, serv);
    }

    bool API::Disconnect(sys::Service *serv)
    {
        std::shared_ptr<InternetRequestMessage> msg =
            std::make_shared<InternetRequestMessage>(MessageType::FotaInternetDisconnect);
        return sys::Bus::SendUnicast(std::move(msg), FotaService::Service::serviceName, serv);
    }

    void API::HTTPGET(sys::Service *serv, const std::string &url)
    {
        LOG_DEBUG("HTTP GET API called");
        std::shared_ptr<HTTPRequestMessage> msg = std::make_shared<HTTPRequestMessage>();
        msg->url                                = url;
        msg->method                             = FotaService::HTTPMethod::GET;
        sys::Bus::SendUnicast(std::move(msg), FotaService::Service::serviceName, serv);
    }

    void API::FotaStart(sys::Service *serv, const string &url)
    {
        LOG_DEBUG("Fota Star: %s", url.c_str());
        std::shared_ptr<FotaService::FOTAStart> msg = std::make_shared<FotaService::FOTAStart>();

        msg->url = url;

        sys::Bus::SendUnicast(std::move(msg), FotaService::Service::serviceName, serv);
    }

    std::string APN::toString(APN::AuthMethod authMethod)
    {
        switch (authMethod) {
        case AuthMethod::NONE:
            return "NONE";
        case AuthMethod::PAP:
            return "PAP";
        case AuthMethod::CHAP:
            return "CHAP";
        case AuthMethod::AUTO:
            return "AUTO";
        }
        return std::to_string(static_cast<int>(authMethod));
    }

    std::string APN::Config::toString() const
    {
        std::ostringstream out;
        out << static_cast<int>(contextId) << "," << (type == ContextType::ipv4 ? "ipv4" : "ipv4v6") << "," << apn
            << "," << username << "," << password << "," << APN::toString(authMethod) << ","
            << (activated ? "Activated" : "Deactivated") << "," << ip << ",";
        return out.str();
    }

    std::string toString(HTTPErrors error)
    {
        switch (error) {
        case HTTPErrors::OK:
            return "OK";
        case HTTPErrors::UnknowError:
            return "UnknowError";
        case HTTPErrors::OpenFailed:
            return "OpenFailed";
        case HTTPErrors::URLFailed:
            return "URLFailed";
        case HTTPErrors::GetFailed:
            return "GetFailed";
        }
        return "unknown (" + std::to_string(static_cast<int>(error)) + ")";
    }

} // namespace FotaService
