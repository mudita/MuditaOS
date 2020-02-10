#include "log/log.hpp"
#include "ParserUtils.hpp"


bool ParserUtils::isMethodValid(uint8_t method)
{
    if(method == static_cast<uint8_t>(Method::Get)
    || method == static_cast<uint8_t>(Method::Post)
    || method == static_cast<uint8_t>(Method::Put)
    || method == static_cast<uint8_t>(Method::Delete))
    {
        return true;
    }
    else
    {
        LOG_ERROR("Invalid method!");
        return false;
    }
}