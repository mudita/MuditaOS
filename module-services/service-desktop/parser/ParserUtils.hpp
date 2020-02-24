#pragma once

#include <stdint.h>
#include <string>

namespace parserutils
{
    /*! Enum class for endpoints.
     */
    enum class Endpoint
    {
        battery,
        backups,
        deviceInfo,
        network,
        storage
    };

    namespace message
    {
        constexpr size_t size_length = 9;
        namespace type
        {
            constexpr uint8_t endpoint = '#';
            constexpr uint8_t rawData = '$';
        } // namespace type
    }     // namespace message

    namespace http
    {
        /*! Enum class for the HTTP status codes.
         */
        enum class Code
        {
            OK = 200,
            BadRequest = 400,
            InternalServerError = 500
        };

        /*! Enum class for the HTTP methods.
         */
        enum class Method
        {
            get,
            post,
            put,
            del
        };

        bool isMethodValid(uint8_t);
    }; // namespace http

    namespace json
    {
        const inline std::string method = "method";
        const inline std::string endpoint = "endpoint";
        const inline std::string status = "status";
        const inline std::string body = "body";

    } // namespace json
};    // namespace parserutils
