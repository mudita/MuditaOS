/*
 *  @file InternetServiceAPI.cpp
 *  @author Lukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 *  @date 16.09.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include <sstream>
#include <string>

#include "InternetServiceAPI.hpp"
#include "Service/Bus.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "base64/base64.hpp"

#if defined(__cplusplus)
extern "C"
{
#endif
#include "memory/usermem.h"
#if defined(__cplusplus)
}
#endif

bool InternetServiceAPI::Connect(sys::Service *serv)
{
    std::shared_ptr<InternetRequestMessage> msg =
        std::make_shared<InternetRequestMessage>(MessageType::InternetConfigure);
    msg->apn = "internet";
    LOG_DEBUG("!!!Internet Config called");

    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return false;

    msg = std::make_shared<InternetRequestMessage>(MessageType::InternetConnect);
    return sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

bool InternetServiceAPI::Disconnect(sys::Service *serv)
{
    std::shared_ptr<InternetRequestMessage> msg =
        std::make_shared<InternetRequestMessage>(MessageType::InternetDisconnect);

    return sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

std::string InternetServiceAPI::HTTPGET(sys::Service *serv, const std::string &url, const std::string &request)
{
    std::shared_ptr<InternetRequestMessage> msg =
        std::make_shared<InternetRequestMessage>(MessageType::InternetOpenHTTPConnection);
    LOG_DEBUG("HTTP GET API called");

    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return "HTTP Open failed";

    msg      = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPUrl);
    msg->url = url;
    ret      = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return "HTTP URL failed";

    msg          = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPGET);
    msg->request = request;
    ret          = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return "HTTP GET failed";

    msg = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPReadData);
    ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return "HTTP Done";

    return "HTTP ERROR";
}

std::string InternetServiceAPI::HTTPPOST(sys::Service *serv,
                                         const std::string &url,
                                         const std::string &request,
                                         const std::string &audio)
{
    std::shared_ptr<InternetRequestMessage> msg =
        std::make_shared<InternetRequestMessage>(MessageType::InternetOpenHTTPConnection);
    LOG_DEBUG("HTTP POST API called");

    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return "HTTP Open failed";

    msg      = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPUrl);
    msg->url = url;
    ret      = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return "HTTP URL failed";

    msg          = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPPOST);
    msg->request = request;
    msg->data    = audio;
    ret          = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return "HTTP POST failed";

    msg = std::make_shared<InternetRequestMessage>(MessageType::InternetHTTPReadData);
    ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    if (!ret)
        return "HTTP Done";

    return "HTTP ERROR";
}

std::string InternetServiceAPI::json(
    encodings encoding, int samplerate, languages language, uint8_t *buffer, uint32_t length)
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
