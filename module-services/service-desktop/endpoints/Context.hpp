// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-services/service-desktop/parser/ParserUtils.hpp>
#include <utility>
#include "ResponseContext.hpp"

namespace parserFSM
{
    constexpr http::Code toCode(bool r)
    {
        return r ? http::Code::OK : http::Code::InternalServerError;
    }

    namespace json
    {
        inline constexpr auto method     = "method";
        inline constexpr auto endpoint   = "endpoint";
        inline constexpr auto uuid       = "uuid";
        inline constexpr auto status     = "status";
        inline constexpr auto totalCount = "totalCount";
        inline constexpr auto body       = "body";
        inline constexpr auto offset     = "offset";
        inline constexpr auto limit      = "limit";
        inline constexpr auto nextPage   = "nextPage";
        inline constexpr auto entries    = "entries";
    } // namespace json

    constexpr int invalidUuid = 0;

    class Context
    {
      protected:
        json11::Json body;
        EndpointType endpoint;
        int uuid;
        http::Method method;
        endpoint::ResponseContext responseContext;

        auto validate() -> void
        {
            if (!body.is_object()) {
                body = json11::Json();
            }
            if (static_cast<int>(endpoint) > lastEndpoint) {
                endpoint = EndpointType::invalid;
            }
            if (method > http::Method::del) {
                method = http::Method::get;
            }
        }
        auto buildResponseStr(std::size_t responseSize, const std::string &responsePayloadString) -> std::string
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
        explicit Context(json11::Json &js)
        {
            body     = js[json::body];
            endpoint = static_cast<EndpointType>(js[json::endpoint].int_value());
            uuid     = js[json::uuid].int_value();
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
        virtual ~Context() noexcept = default;

        virtual auto createSimpleResponse(const std::string &entryTitle = json::entries) -> std::string
        {
            json11::Json::object contextJsonObject =
                json11::Json::object{{json::endpoint, static_cast<int>(getEndpoint())},
                                     {json::status, static_cast<int>(responseContext.status)},
                                     {json::uuid, getUuid()}};
            if (!responseContext.body.is_null()) {
                contextJsonObject[json::body] = responseContext.body;
            }

            const json11::Json responseJson{std::move(contextJsonObject)};

            return buildResponseStr(responseJson.dump().size(), responseJson.dump());
        }

        auto setResponse(endpoint::ResponseContext r)
        {
            responseContext = std::move(r);
        }

        auto setResponseStatus(http::Code status)
        {
            responseContext.status = status;
        }
        auto setEndpoint(EndpointType endpointTypeToSet)
        {
            endpoint = endpointTypeToSet;
        }
        auto setResponseBody(json11::Json respBody)
        {
            responseContext.body = std::move(respBody);
        }
        auto getBody() -> const json11::Json &
        {
            return body;
        }
        auto getEndpoint() -> EndpointType
        {
            return endpoint;
        }
        auto getUuid() -> int
        {
            return uuid;
        }
        auto getMethod() -> http::Method
        {
            return method;
        }
    };

    class PagedContext : public Context
    {
      private:
        // from request
        std::size_t requestedLimit{}, requestedOffset{};
        // set by query (during helper run)
        std::size_t totalCount{};
        // set it before calling handle on helper
        std::size_t pageSize{};

      public:
        explicit PagedContext(json11::Json &js, size_t pageSize) : Context(js), pageSize(pageSize)
        {}
        PagedContext() = default;
        void setRequestedLimit(std::size_t limit)
        {
            requestedLimit = limit;
        }
        void setRequestedOffset(std::size_t offset)
        {
            requestedOffset = offset;
        }
        void setTotalCount(std::size_t count)
        {
            totalCount = count;
        }
        std::size_t getPageSize() const
        {
            return pageSize;
        }

        auto createSimpleResponse(const std::string &entryTitle = json::entries) -> std::string override
        {
            auto elemsCount = responseContext.body.array_items().size();
            auto newBody    = json11::Json::object{{entryTitle, responseContext.body},
                                                {json::totalCount, static_cast<int>(totalCount)}};
            if (requestedLimit > elemsCount) {
                std::size_t offset = requestedOffset + elemsCount;
                if (offset < totalCount) {
                    auto lastTableIndex = std::min(totalCount, offset + requestedLimit - elemsCount);
                    std::size_t limit   = std::min(pageSize, lastTableIndex - offset);
                    auto nextPageParams = json11::Json::object{{json::offset, static_cast<int>(offset)},
                                                               {json::limit, static_cast<int>(limit)}};
                    newBody.insert({json::nextPage, nextPageParams});
                }
            }

            setResponseBody(newBody);
            return Context::createSimpleResponse();
        }
    };

    namespace endpoint_pageing
    {
        inline constexpr std::size_t contactsPageSize       = 10;
        inline constexpr std::size_t messagesPageSize       = 4;
    } // namespace endpoint_pageing

    class ContextFactory
    {
      public:
        static auto create(json11::Json &js) -> std::unique_ptr<parserFSM::Context>
        {
            switch (static_cast<EndpointType>(js[json::endpoint].int_value())) {
            // enable for pagination in other endpoints
            // case EndpointType::calllog:
            case EndpointType::contacts:
                return std::make_unique<PagedContext>(js, endpoint_pageing::contactsPageSize);
            case EndpointType::messages:
                return std::make_unique<PagedContext>(js, endpoint_pageing::messagesPageSize);
            default:
                return std::make_unique<Context>(js);
            }
        }
    };

} // namespace parserFSM
