#pragma once
#include "json/json11.hpp"
#include "module-services/service-desktop/parser/ParserUtils.hpp"

namespace parserFSM
{

    namespace json
    {
        const inline std::string method   = "method";
        const inline std::string endpoint = "endpoint";
        const inline std::string uuid     = "uuid";
        const inline std::string status   = "status";
        const inline std::string body     = "body";
    } // namespace json

    struct endpointResponseContext
    {
        http::Code status = http::Code::OK;
        json11::Json body = json11::Json();
    };

    constexpr int invalidUuid = 0;

    class Context
    {
      private:
        json11::Json body;
        EndpointType endpoint;
        uint32_t uuid;
        http::Method method;
        endpointResponseContext responseContext;

        auto validate() -> void
        {
            if (body.is_object() == false) {
                body = json11::Json();
            }
            if (static_cast<int>(endpoint) > lastEndpoint) {
                endpoint = EndpointType::invalid;
            }
            if (method > http::Method::del) {
                method = http::Method::get;
            }
        }
        auto buildResponseStr(std::size_t responseSize, std::string responsePayloadString) -> std::string
        {
            constexpr auto pos                 = 0;
            constexpr auto count               = 1;
            std::string responsePayloadSizeStr = std::to_string(responseSize);
            while (responsePayloadSizeStr.length() < message::size_length) {
                responsePayloadSizeStr.insert(pos, count, '0');
            }

            std::string responseStr = message::endpointChar + responsePayloadSizeStr + responsePayloadString;
            return responseStr;
        }

      public:
        Context(json11::Json &js)
        {
            body     = js[json::body];
            endpoint = static_cast<EndpointType>(js[json::endpoint].int_value());
            uuid     = js[json::uuid].int_value();
            if (uuid == invalidUuid) {
                try {
                    uuid = stoi(js[json::uuid].string_value());
                }
                catch (...) {
                    uuid = invalidUuid;
                }
            }
            method   = static_cast<http::Method>(js[json::method].int_value());
            validate();
        }
        Context()
        {
            body     = json11::Json();
            endpoint = EndpointType::invalid;
            uuid     = invalidUuid;
            method   = http::Method::get;
        }

        auto createSimpleResponse() -> std::string
        {
            json11::Json responseJson = json11::Json::object{{json::endpoint, static_cast<int>(getEndpoint())},
                                                             {json::status, static_cast<int>(responseContext.status)},
                                                             {json::uuid, std::to_string(getUuid())},
                                                             {json::body, responseContext.body}};
            return buildResponseStr(responseJson.dump().size(), responseJson.dump());
        }
        auto setResponseStatus(http::Code status)
        {
            responseContext.status = status;
        }
        auto setResponseBody(json11::Json respBody)
        {
            responseContext.body = respBody;
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

} // namespace parserFSM
