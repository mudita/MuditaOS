// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <json11.hpp>

namespace sdesktop::endpoints
{

    class EndpointFactory;

    class MessageHandler
    {
        json11::Json messageJson;
        std::string JsonErrorMsg;
        sys::Service *OwnerServicePtr = nullptr;
        std::unique_ptr<EndpointFactory> endpointFactory;

      public:
        MessageHandler(sys::Service *OwnerService, std::unique_ptr<EndpointFactory> endpointFactory);

        [[nodiscard]] auto isJSONNull() const -> bool
        {
            return messageJson.is_null();
        };
        [[nodiscard]] auto isValid() const noexcept -> bool
        {
            return JsonErrorMsg.empty();
        }
        [[nodiscard]] auto getErrorString() const -> const std::string &
        {
            return JsonErrorMsg;
        };
        void parseMessage(const std::string &msg);
        void processMessage();
    };

} // namespace sdesktop::endpoints
