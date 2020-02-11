#pragma once
#include <Service/Service.hpp>

namespace harness
{
    namespace action
    {
        bool gsm_send(sys::Service *serv, const std::string &cmd);
    }
} // namespace harness
