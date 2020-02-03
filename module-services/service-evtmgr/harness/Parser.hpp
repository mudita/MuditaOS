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
