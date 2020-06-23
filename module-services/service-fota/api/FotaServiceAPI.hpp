#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace sys
{
    class Service;
}

namespace FotaService
{
    namespace APN
    {
        /**
         * @brief Defines ip address type as in "TCP(IP) AT Commands Manual"
         */
        enum class ContextType : unsigned char
        {
            ipv4   = 1,
            ipv4v6 = 2
        };

        /**
         * @brief APN Authorization method
         */
        enum class AuthMethod : char
        {
            NONE = 0,
            PAP  = 1,
            CHAP = 2,
            AUTO = 3,
        };
        std::string toString(AuthMethod authMethod);
        /**
         * @brief APN configuration data
         */
        class Config
        {
          public:
            unsigned char contextId =
                0; /// context on which apn is configured available values 1-16, 0 - means not set yet
            bool activated        = false;
            ContextType type      = ContextType::ipv4; /// IP type
            AuthMethod authMethod = AuthMethod::NONE;
            std::string apn; /// name of the APN
            std::string username;
            std::string password;
            std::string ip; /// set after connection

            std::string toString() const;
        };
        using ContextMap  = std::unordered_map<unsigned char, APN::Config>;
        using ContextPair = std::pair<unsigned char, APN::Config>;
    } // namespace APN

    enum class HTTPErrors
    {
        OK,
        UnknowError,
        OpenFailed,
        URLFailed,
        GetFailed,
    };

    std::string toString(HTTPErrors error);
    enum class HTTPMethod
    {
        GET,
        POST,
    };

    class API
    {
      public:
        static bool Configure(sys::Service *serv, const APN::Config &config);
        static bool Connect(sys::Service *serv);
        static bool Disconnect(sys::Service *serv);
        /**
         * @brief HTTPGET downloads a file,
         * @param serv - service where response should be send
         * @param url  - url to connect to (automatically detects https)
         *
         * This function is asynchronius.
         * Pirior to calling this function serv must be connected to @a FoataService::HTTPResponse message,
         * otherwise Application will not receive data.
         * @return
         */
        static void HTTPGET(sys::Service *serv, const std::string &url);
        static void FotaStart(sys::Service *serv, const std::string &url);
        static void sendRawProgress(sys::Service *serv, const std::string &rawQind);
    };
} // namespace FotaService
