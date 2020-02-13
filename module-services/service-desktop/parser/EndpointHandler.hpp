#pragma once

#include <string>
#include "Service/Common.hpp"

class EndpointHandler
{
private:
    std::string rspPayloadSizeToStr(std::size_t);

public:
  sys::ReturnCodes battery(uint8_t);
};
