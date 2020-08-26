#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <log/log.hpp>

namespace parserFSM
{

    // Endpoint type definition
    enum class EndpointType
    {
        invalid = 0,
        deviceInfo,
        update,
        backup,
        restore,
        factory,
        contacts,
        messages
    };

    constexpr int lastEndpoint = static_cast<int>(EndpointType::messages);
    // Message defs and utils
    namespace message
    {
        constexpr size_t size_length = 9;
        constexpr size_t size_header = size_length + 1;

        constexpr char endpointChar = '#';
        constexpr char rawDataChar  = '$';

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
            BadRequest          = 400,
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
        const inline std::string batteryLevel   = "batteryLevel";
        const inline std::string batteryState   = "batteryState";
        const inline std::string selectedSim    = "selectedSim";
        const inline std::string trayState      = "trayState";
        const inline std::string signalStrength = "signalStrength";
        const inline std::string fsTotal        = "fsTotal";
        const inline std::string fsFreePercent  = "fsFreePercent";
        const inline std::string fsFree         = "fsFree";
        const inline std::string gitRevision    = "gitRevision";
        const inline std::string gitBranch      = "gitBranch";
        const inline std::string gitTag         = "gitTag";
        const inline std::string currentRTCTime = "currentRTCTime";
        const inline std::string updateReady    = "updateReady";
        const inline std::string updateFileList = "updateFileList";
        const inline std::string backupRequest  = "backupRequest";
        const inline std::string backupReady    = "backupReady";
        const inline std::string backupUpload   = "backupUpload";
        const inline std::string restoreRequest = "restoreRequest";
        const inline std::string factoryRequest = "factoryRequest";

        namespace messages
        {
            const inline std::string id           = "id";
            const inline std::string count        = "count";
            const inline std::string offset       = "offset";
            const inline std::string phoneNumber  = "phoneNumber";
            const inline std::string messageBody  = "messageBody";
            const inline std::string isUnread     = "unread";
            const inline std::string contactID    = "contactID";
            const inline std::string date         = "date";
            const inline std::string dateSent     = "dateSent";
            const inline std::string type         = "type";
            const inline std::string threadID     = "threadID";
            const inline std::string msgTemplate  = "template";
            const inline std::string templateText = "text";
            namespace thread
            {
                const inline std::string msgCount       = "msgCount";
                const inline std::string snippet        = "snippet";
                const inline std::string unreadMsgCount = "unreadMsgCount";

            } // namespace thread

        } // namespace messages

    } // namespace json

} // namespace parserFSM
