#pragma once

#include "Events.hpp"
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <string>

namespace harness
{

    enum class Error
    {
        Success,
        Generic,
        BadRequest,   /// request is not { ... }
        NoType,       /// no Event type in request
        NotHandled,   /// event is registered but wasn't handled
        ParserFailed, /// error on parsing
    };

    Error parse(const std::string &request, sys::Service *serv);
}; // namespace harness

inline const char *c_str(harness::Error code)
{
    switch (code) {
    case harness::Error::Success:
        return "Success";
    case harness::Error::Generic:
        return "Generic";
    case harness::Error::BadRequest:
        return "BadRequest";
    case harness::Error::NoType:
        return "NoType";
    case harness::Error::NotHandled:
        return "NotHandled";
    case harness::Error::ParserFailed:
        return "ParserFailed";
    }
    return "";
}
