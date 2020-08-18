#pragma once
#include "ParserUtils.hpp"
#include "json/json11.hpp"

namespace ParserStateMachine
{

    namespace json
    {
        const inline std::string method   = "method";
        const inline std::string endpoint = "endpoint";
        const inline std::string uuid     = "uuid";
        const inline std::string status   = "status";
        const inline std::string body     = "body";
    } // namespace json

    class Context
    {
      private:
        json11::Json body;
        EndpointType endpoint;
        uint32_t uuid;
        http::Method method;
        auto validate() -> void
        {
            if (body.is_object() == false) {
                body = json11::Json();
            }
            if (static_cast<int>(endpoint) > lastEndpoint) {
                endpoint = EndpointType::invalid;
            }
        }

      public:
        Context(json11::Json &js)
        {
            body     = js[json::body];
            endpoint = static_cast<EndpointType>(js[json::endpoint].int_value());
            uuid     = js[json::uuid].int_value();
            method   = static_cast<http::Method>(js[json::method].int_value());
            validate();
        }
        auto getBody() -> json11::Json
        {
            return body;
        }
        auto getEndpoint() -> EndpointType
        {
            return endpoint;
        }
        auto getUuid() -> uint32_t
        {
            return uuid;
        }
        auto getMethod() -> http::Method
        {
            return method;
        }
    };

} // namespace ParserStateMachine
