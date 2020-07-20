#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <log/log.hpp>

namespace ParserStateMachine
{

    // Endpoint type definition
    enum class Endpoint
    {
        deviceInfo = 1,
        update,
        backup,
        restore,
        factory,
        contacts
    };

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
        const inline std::string method   = "method";
        const inline std::string endpoint = "endpoint";
        const inline std::string uuid     = "uuid";
        const inline std::string status   = "status";
        const inline std::string body     = "body";

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

        namespace contacts
        {
            const inline std::string count           = "count";
            const inline std::string primaryName     = "priName";
            const inline std::string alternativeName = "altName";
            const inline std::string address         = "address";
            const inline std::string id              = "id";
            const inline std::string numbers         = "numbers";
            const inline std::string isBlocked       = "blocked";
            const inline std::string isFavourite     = "favourite";

        } // namespace contacts

    } // namespace json

} // namespace ParserStateMachine
