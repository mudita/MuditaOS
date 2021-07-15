// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Context.hpp"

namespace parserFSM
{
    auto Context::createSimpleResponse(const std::string &entryTitle) -> std::string
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

    auto Context::createErrorResponse(http::Code code, const std::string &text) -> std::string
    {
        setResponseStatus(code);
        setResponseBody(std::map<std::string, std::string>{{"error", text}});
        return createSimpleResponse();
    }
} // namespace parserFSM
