#pragma once
#include "ParserUtils.hpp"
#include "json/json11.hpp"
#include "Common/Query.hpp"
#include "Context.hpp"
#include "Service/Service.hpp"
#include <string>

namespace ParserStateMachine
{

    class Endpoint
    {

      public:
        Endpoint(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        virtual ~Endpoint()                                  = default;
        virtual auto handle(Context &context) -> std::string = 0;
        auto c_str() -> const char *
        {
            return debugName.c_str();
        }

        static auto buildResponseStr(std::size_t responseSize, std::string responsePayloadString) -> std::string
        {
            std::string responsePayloadSizeStr = std::to_string(responseSize);
            while (responsePayloadSizeStr.length() < message::size_length) {
                responsePayloadSizeStr.insert(0, 1, '0');
            }

            std::string responseStr = message::endpointChar + responsePayloadSizeStr + responsePayloadString;
            return responseStr;
        }

        static auto createSimpleResponse(sys::ReturnCodes status, int endpoint, uint32_t uuid, json11::Json body)
            -> std::string
        {
            json11::Json responseJson = json11::Json::object{
                {json::endpoint, endpoint},
                {json::status,
                 static_cast<int>(status == sys::ReturnCodes::Success ? http::Code::OK
                                                                      : http::Code::InternalServerError)},
                {json::uuid, std::to_string(uuid)},
                {json::body, body}};
            return Endpoint::buildResponseStr(responseJson.dump().size(), responseJson.dump());
        }

        static auto createSimpleResponse(bool status, int endpoint, uint32_t uuid, json11::Json body) -> std::string
        {
            json11::Json responseJson = json11::Json::object{
                {json::endpoint, endpoint},
                {json::status, static_cast<int>(status ? http::Code::OK : http::Code::InternalServerError)},
                {json::uuid, std::to_string(uuid)},
                {json::body, body}};

            return Endpoint::buildResponseStr(responseJson.dump().size(), responseJson.dump());
        }

      protected:
        std::string debugName;
        sys::Service *ownerServicePtr;
    };

} // namespace ParserStateMachine
