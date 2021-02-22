﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-services/service-desktop/parser/ParserUtils.hpp>
#include "ResponseContext.hpp"

namespace parserFSM
{

    namespace json
    {
        inline constexpr auto method   = "method";
        inline constexpr auto endpoint = "endpoint";
        inline constexpr auto uuid     = "uuid";
        inline constexpr auto status   = "status";
        inline constexpr auto totalCount = "totalCount";
        inline constexpr auto body     = "body";
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
        uint32_t uuid;
        http::Method method;
        endpoint::ResponseContext responseContext;

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
            method = static_cast<http::Method>(js[json::method].int_value());
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
            json11::Json responseJson = json11::Json::object{{json::endpoint, static_cast<int>(getEndpoint())},
                                                             {json::status, static_cast<int>(responseContext.status)},
                                                             {json::uuid, std::to_string(getUuid())},
                                                             {json::body, responseContext.body}};
            return buildResponseStr(responseJson.dump().size(), responseJson.dump());
        }

        auto setResponse(endpoint::ResponseContext r)
        {
            r = responseContext;
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
            responseContext.body = respBody;
        }
        auto getBody() -> const json11::Json &
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

    class PagedContext : public Context
    {
      private:
        // from request
        std::size_t requestedLimit, requestedOffset;
        // set by query (during helper run)
        std::size_t totalCount;
        // set it before calling handle on helper
        std::size_t pageSize;

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
        inline constexpr std::size_t contactsPageSize = 10;
        inline constexpr std::size_t calendarEventsPageSize = 10;
    }

    class ContextFactory
    {
      public:
        static auto create(json11::Json &js) -> std::unique_ptr<parserFSM::Context>
        {
            switch (static_cast<EndpointType>(js[json::endpoint].int_value())) {
            // enable for pagination in other endpoints
            case EndpointType::calendarEvents:
                return std::make_unique<PagedContext>(js, endpoint_pageing::calendarEventsPageSize);
            // case EndpointType::calllog:
            case EndpointType::contacts:
                // case EndpointType::messages:
                return std::make_unique<PagedContext>(js, endpoint_pageing::contactsPageSize);
            default:
                return std::make_unique<Context>(js);
            }
        }
    };

} // namespace parserFSM
