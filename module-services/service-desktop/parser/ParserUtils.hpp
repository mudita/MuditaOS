// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stdint.h>         // for uint8_t
#include <log/log.hpp>      // for LOG_ERROR
#include <bits/exception.h> // for exception
#include <stddef.h>         // for size_t
#include <string>           // for string, allocator, basic_string, stol
#include <vector>

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
        developerMode,
        deviceUpdate
    };

    inline constexpr auto lastEndpoint = static_cast<int>(EndpointType::developerMode);
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

        inline unsigned long calcPayloadLength(const std::string header)
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
            if (msg.size() > payloadLength)
                return msg.substr(0, payloadLength);
            else
                return msg;
        }
    } // namespace message

    namespace http
    {
        /*! Enum class for the HTTP status codes.
         */
        enum class Code
        {
            OK                  = 200,
            Accepted            = 202,
            BadRequest          = 400,
            NotAcceptable       = 406,
            InternalServerError = 500
        };

        /*! Enum class for the HTTP methods.
         */
        enum class Method
        {
            get = 1,
            post,
            put,
            del
        };

        bool isMethodValid(uint8_t);
    }; // namespace http

    namespace json
    {
        inline constexpr auto batteryLevel     = "batteryLevel";
        inline constexpr auto batteryState     = "batteryState";
        inline constexpr auto selectedSim      = "selectedSim";
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
        inline constexpr auto backupRequest    = "backupRequest";
        inline constexpr auto backupReady      = "backupReady";
        inline constexpr auto backupUpload     = "backupUpload";
        inline constexpr auto restoreRequest   = "restoreRequest";
        inline constexpr auto factoryRequest   = "factoryRequest";
        inline constexpr auto networkStatus    = "networkStatus";
        inline constexpr auto accessTechnology = "accessTechnology";
        inline constexpr auto fileName         = "fileName";
        inline constexpr auto fileSize         = "fileSize";

        inline constexpr auto update      = "update";
        inline constexpr auto updateInfo  = "updateInfo";
        inline constexpr auto updateError = "updateError";
        inline constexpr auto errorCode   = "errorCode";
        inline constexpr auto statusCode  = "statusCode";

        namespace filesystem
        {
            inline constexpr auto command = "command";
            namespace commands
            {
                inline constexpr auto upload   = "upload";
                inline constexpr auto rm       = "rm";
                inline constexpr auto download = "download";
            } // namespace commands
        }     // namespace filesystem

        namespace messages
        {
            inline constexpr auto id           = "id";
            inline constexpr auto count        = "count";
            inline constexpr auto offset       = "offset";
            inline constexpr auto phoneNumber  = "phoneNumber";
            inline constexpr auto messageBody  = "messageBody";
            inline constexpr auto isUnread     = "unread";
            inline constexpr auto contactID    = "contactID";
            inline constexpr auto date         = "date";
            inline constexpr auto dateSent     = "dateSent";
            inline constexpr auto type         = "type";
            inline constexpr auto threadID     = "threadID";
            inline constexpr auto msgTemplate  = "template";
            inline constexpr auto templateText = "text";
            namespace thread
            {
                inline constexpr auto msgCount       = "msgCount";
                inline constexpr auto snippet        = "snippet";
                inline constexpr auto unreadMsgCount = "unreadMsgCount";

            } // namespace thread

        } // namespace messages

    } // namespace json

} // namespace parserFSM
