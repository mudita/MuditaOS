#include "InternetServiceAPI.hpp"

#include <service-internet/ServiceInternet.hpp>
#include <service-internet/messages/InternetMessage.hpp>
#include <service-cellular/messages/CellularMessage.hpp>
#include <service-cellular/ServiceCellular.hpp>

#include <Service/Bus.hpp>
#include <base64/base64.hpp>

#include <sstream>
#include <string>

namespace InternetService
{

    bool API::Configure(sys::Service *serv, const APN::Config &config)
    {
        std::shared_ptr<ConfigureAPNMessage> msg = std::make_shared<ConfigureAPNMessage>(config);
        LOG_DEBUG("!!!Internet Config called");
        return sys::Bus::SendUnicast(msg, InternetService::Service::serviceName, serv);
    }

    bool API::Connect(sys::Service *serv)
    {
        LOG_DEBUG("!!!Internet connection called");
        auto msg = std::make_shared<ConnectMessage>();
        return sys::Bus::SendUnicast(msg, InternetService::Service::serviceName, serv);
    }

    bool API::Disconnect(sys::Service *serv)
    {
        std::shared_ptr<InternetRequestMessage> msg =
            std::make_shared<InternetRequestMessage>(MessageType::InternetDisconnect);
        return sys::Bus::SendUnicast(msg, InternetService::Service::serviceName, serv);
    }

    void API::HTTPGET(sys::Service *serv, const std::string &url)
    {
        LOG_DEBUG("HTTP GET API called");
        std::shared_ptr<HTTPRequestMessage> msg = std::make_shared<HTTPRequestMessage>();
        msg->url                                = url;
        msg->method                             = InternetService::HTTPMethod::GET;
        sys::Bus::SendUnicast(msg, InternetService::Service::serviceName, serv);
    }

    std::string API::HTTPPOST(sys::Service *serv,
                              const std::string &url,
                              const std::string &request,
                              const std::string &audio)
    {
        std::shared_ptr<InternetRequestMessage> msg =
            std::make_shared<InternetRequestMessage>(MessageType::InternetOpenHTTPConnection);
        LOG_DEBUG("HTTP POST API called");

        auto ret = sys::Bus::SendUnicast(msg, InternetService::Service::serviceName, serv);
        if (!ret)
            return "HTTP Open failed";

        msg      = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPUrl);
        msg->url = url;
        ret      = sys::Bus::SendUnicast(msg, InternetService::Service::serviceName, serv);
        if (!ret)
            return "HTTP URL failed";

        msg          = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPPOST);
        msg->request = request;
        msg->data    = audio;
        ret          = sys::Bus::SendUnicast(msg, InternetService::Service::serviceName, serv);
        if (!ret)
            return "HTTP POST failed";

        msg = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPReadData);
        ret = sys::Bus::SendUnicast(msg, InternetService::Service::serviceName, serv);
        if (!ret)
            return "HTTP Done";

        return "HTTP ERROR";
    }

    std::string API::json(encodings encoding, int samplerate, languages language, uint8_t *buffer, uint32_t length)
    {
        std::string enc;
        std::string lang;
        std::stringstream tmp2;
        std::string ret;
        std::string tmpstr;

        switch (encoding) {
        default:
        case encodings::encUNSPECIFIED:
            enc = "ENCODING_UNSPECIFIED";
            break;
        case encodings::encLINEAR16:
            enc = "LINEAR16";
            break;
        case encodings::encFLAC:
            enc = "FLAC";
            break;
        case encodings::encMULAW:
            enc = "MULAW";
            break;
        case encodings::encAMR:
            enc = "AMR";
            break;
        case encodings::encAMR_WB:
            enc = "AMR_WB";
            break;
        case encodings::encOGG:
            enc = "OGG_OPUS";
            break;
        case encodings::encSPEEX:
            enc = "SPEEX_WITH_HEADER_BYTE";
            break;
        }

        switch (language) {
        default:
        case languages::langEN:
            lang = "en-US";
            break;
        case languages::langPL:
            lang = "pl-PL";
            break;
        case languages::langES:
            lang = "eu_ES";
            break;
        }

        std::string b64audio = base64_encode(buffer, length);

        tmp2 << b64audio;

        // assemble json string
        std::stringstream tmp;
        tmp << samplerate;
        tmpstr = tmp2.str();
        tmpstr.pop_back(); // remove last character - it's '\n' which is confusing base64 decoder
        ret = "{\n"
              "  \"config\": {\n"
              "    \"encoding\": \"" +
              enc +
              "\",\n"
              "    \"sampleRateHertz\": " +
              tmp.str() +
              ",\n"
              "    \"languageCode\": \"" +
              lang +
              "\",\n"
              "    \"enableWordTimeOffsets\": false\n"
              "  },\n"
              "  \"audio\": {\n"
              "    \"content\": \"" +
              tmpstr +
              "\"\n"
              "  }\n"
              "}\n";
        return ret;
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

} // namespace InternetService
