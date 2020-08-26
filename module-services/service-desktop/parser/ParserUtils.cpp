#include "ParserUtils.hpp"

using namespace parserFSM;

bool http::isMethodValid(uint8_t method)
{
    if (method == static_cast<uint8_t>(http::Method::get) || method == static_cast<uint8_t>(http::Method::post) ||
        method == static_cast<uint8_t>(http::Method::put) || method == static_cast<uint8_t>(http::Method::del)) {
        return true;
    }
    else {
        LOG_ERROR("Invalid method!");
        return false;
    }
}
