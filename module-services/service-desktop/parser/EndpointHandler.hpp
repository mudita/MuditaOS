#pragma once

#include <string>
#include "Service/Common.hpp"

class EndpointHandler
{
  public:
    sys::ReturnCodes battery(uint8_t httpMethod, std::string &responseStr);
    sys::ReturnCodes backups(uint8_t httpMethod, std::string &responseStr);
    sys::ReturnCodes deviceInfo(uint8_t httpMethod, std::string &responseStr);
    sys::ReturnCodes network(uint8_t httpMethod, std::string &responseStr);
    sys::ReturnCodes storage(uint8_t httpMethod, std::string &responseStr);

  private:
    std::string buildResponseStr(std::size_t responseSize, std::string responsePayloadString);
};
