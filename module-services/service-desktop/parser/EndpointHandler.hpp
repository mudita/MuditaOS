#pragma once

#include <json/json11.hpp>
#include <Service/Service.hpp>
#include "ParserUtils.hpp"
#include <service-desktop/ServiceDesktop.hpp>

class EndpointHandler
{
  public:
    static std::string buildResponseStr(std::size_t responseSize, std::string responsePayloadString);
    sys::ReturnCodes update(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    sys::ReturnCodes deviceInfo(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    sys::ReturnCodes backup(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    sys::ReturnCodes restore(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    sys::ReturnCodes factory(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
};
