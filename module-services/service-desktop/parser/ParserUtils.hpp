// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <log/log.hpp>      // for LOG_ERROR
#include <bits/exception.h> // for exception
#include <cstddef>          // for size_t
#include <string>           // for string, allocator, basic_string, stol
#include <vector>
#include <parser/HttpEnums.hpp>

namespace parserFSM
{

    // Endpoint type definition
    enum class EndpointType
    {
        invalid = 0,
        deviceInfo,
        update,
        filesystemUpload,
        backup,
        restore,
        factory,
        contacts,
        messages,
        calllog,
        calendarEvents,
        developerMode,
        bluetooth
    };

    inline constexpr auto lastEndpoint = static_cast<int>(EndpointType::bluetooth);
    // Message defs and utils
    namespace message
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
    } // namespace message

    namespace json
    {
        inline constexpr auto batteryLevel     = "batteryLevel";
        inline constexpr auto batteryState     = "batteryState";
        inline constexpr auto selectedSim      = "selectedSim";
        inline constexpr auto sim              = "sim";
        inline constexpr auto trayState        = "trayState";
        inline constexpr auto signalStrength   = "signalStrength";
        inline constexpr auto fsTotal          = "fsTotal";
        inline constexpr auto fsFreePercent    = "fsFreePercent";
        inline constexpr auto fsFree           = "fsFree";
        inline constexpr auto gitRevision      = "gitRevision";
        inline constexpr auto gitBranch        = "gitBranch";
        inline constexpr auto gitTag           = "gitTag";
        inline constexpr auto currentRTCTime   = "currentRTCTime";
        inline constexpr auto updateReady      = "updateReady";
        inline constexpr auto updateFileList   = "updateFileList";
        inline constexpr auto restoreRequest   = "restoreRequest";
        inline constexpr auto factoryRequest   = "factoryRequest";
        inline constexpr auto networkStatus    = "networkStatus";
        inline constexpr auto accessTechnology = "accessTechnology";
        inline constexpr auto fileName         = "fileName";
        inline constexpr auto fileSize         = "fileSize";
        inline constexpr auto update           = "update";
        inline constexpr auto updateInfo       = "updateInfo";
        inline constexpr auto updateError      = "updateError";
        inline constexpr auto errorCode        = "errorCode";
        inline constexpr auto statusCode       = "statusCode";
        inline constexpr auto updateHistory    = "updateHistory";
        inline constexpr auto fileExists       = "fileExists";
        inline constexpr auto version          = "version";
        inline constexpr auto task             = "task";
        inline constexpr auto state            = "state";
        inline constexpr auto success          = "success";
        inline constexpr auto request          = "request";
        inline constexpr auto finished         = "finished";
        inline constexpr auto pending          = "pending";
        inline constexpr auto location         = "location";
        namespace filesystem
        {
            inline constexpr auto command = "command";
            namespace commands
            {
                inline constexpr auto upload    = "upload";
                inline constexpr auto rm        = "rm";
                inline constexpr auto download  = "download";
                inline constexpr auto checkFile = "checkFile";
            } // namespace commands
        }     // namespace filesystem

        namespace updateprocess
        {
            inline constexpr auto command       = "command";
            inline constexpr auto updateAborted = "updateAborted";
            namespace commands
            {
                inline constexpr auto abort = "abort";
            } // namespace commands
        }     // namespace updateprocess

        namespace messages
        {
            inline constexpr auto count            = "count";
            inline constexpr auto category         = "category";
            inline constexpr auto categoryMessage  = "message";
            inline constexpr auto categoryThread   = "thread";
            inline constexpr auto categoryTemplate = "template";

            inline constexpr auto limit              = "limit";
            inline constexpr auto offset             = "offset";
            inline constexpr auto totalCount         = "totalCount";
            inline constexpr auto nextPage           = "nextPage";
            inline constexpr auto entries            = "entries";
            inline constexpr auto messageBody        = "messageBody";
            inline constexpr auto messageCount       = "messageCount";
            inline constexpr auto messageID          = "messageID";
            inline constexpr auto messageType        = "messageType";
            inline constexpr auto phoneNumber        = "phoneNumber";
            inline constexpr auto receivedAt         = "receivedAt";
            inline constexpr auto sentAt             = "sentAt";
            inline constexpr auto lastUsedAt         = "lastUsedAt";
            inline constexpr auto lastUpdatedAt      = "lastUpdatedAt";
            inline constexpr auto isUnread           = "isUnread";
            inline constexpr auto contactID          = "contactID";
            inline constexpr auto numberID           = "numberID";
            inline constexpr auto threadID           = "threadID";
            inline constexpr auto messageSnippet     = "messageSnippet";
            inline constexpr auto unreadMessageCount = "unreadMessageCount";
            inline constexpr auto messageTemplate    = "messageTemplate";
            inline constexpr auto templateBody       = "templateBody";
            inline constexpr auto templateID         = "templateID";
        } // namespace messages

    } // namespace json

} // namespace parserFSM
