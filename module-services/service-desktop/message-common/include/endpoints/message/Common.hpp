// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <log/log.hpp>
#include <string>
#include <json11.hpp>

namespace sdesktop::endpoints::message
{

    inline constexpr auto size_length = 9U;
    inline constexpr auto size_header = size_length + 1;

    inline constexpr auto endpointChar = '#';
    inline constexpr auto rawDataChar  = '$';

    inline void removeHeader(std::string &msg)
    {
        msg.erase(msg.begin(), msg.begin() + size_header);
    }

    inline unsigned long calcPayloadLength(const std::string &header)
    {
        try {
            return std::stol(header.substr(1, std::string::npos));
        }
        catch (const std::exception &e) {
            LOG_ERROR("[PARSER FSM] Cant calculate payload length: %s", e.what());
            return 0;
        }
    }

    inline std::string getHeader(const std::string &msg)
    {
        return msg.substr(0, size_header);
    }

    inline void eraseFront(std::string &msg, size_t pos)
    {
        msg.erase(msg.begin(), msg.begin() + pos);
    }
    inline std::string extractPayload(std::string &msg, size_t payloadLength)
    {
        if (msg.size() > payloadLength) {
            return msg.substr(0, payloadLength);
        }
        else {
            return msg;
        }
    }

    inline std::unique_ptr<std::string> buildResponse(const json11::Json &msg)
    {
        const auto jsonStr                 = msg.dump();
        const auto pos                     = 0;
        const auto count                   = 1;
        std::string responsePayloadSizeStr = std::to_string(jsonStr.size());

        while (responsePayloadSizeStr.length() < message::size_length) {
            responsePayloadSizeStr.insert(pos, count, '0');
        }

        return std::make_unique<std::string>(message::endpointChar + responsePayloadSizeStr + jsonStr);
    }

} // namespace sdesktop::endpoints::message
