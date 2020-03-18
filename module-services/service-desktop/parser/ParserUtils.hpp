#pragma once

#include <stdint.h>
#include <string>

namespace parserutils
{
    /*! Enum class for endpoints.
     */
    enum class Endpoint
    {
        deviceInfo = 1,
        update
    };

    namespace message
    {
        constexpr size_t size_length = 9;
        /*! Enum class for the message types.
         */
        enum class Type
        {
            endpoint = '#',
            rawData  = '$'
        };
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

    } // namespace json
};    // namespace parserutils
