// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <map>

#include <Utils.hpp>

#include <json11.hpp>

namespace packet_data
{

    namespace json::APN
    {
        namespace Config
        {
            constexpr inline auto contextId   = "contextId";
            constexpr inline auto apnType     = "apnType";
            constexpr inline auto contextType = "contextType";
            constexpr inline auto authMethod  = "authMethod";
            constexpr inline auto apn         = "apn";
            constexpr inline auto username    = "username";
            constexpr inline auto password    = "password";

        } // namespace Config
    }     // namespace json::APN
    enum class DataTransfer
    {
        Off,
        On
    };

    namespace APN
    {
        /**
         * @brief Defines APN types
         */
        enum class APNType
        {
            Default,  ///< only one APN is set as default
            IMS,      ///< IP Multimedia Subsystem for eg VoLTE
            MMS,      ///< for MMS service
            Fota,     ///< for Firmware Update
            Internet, ///< for internet connection, Default have same meaning (but default). If this change to default,
                      ///< existing Default will change to Internet
            Internal  ///< for Internal MBN APNs
        };

        /**
         * @brief Defines ip address type as in "TCP(IP) AT Commands Manual v1.1" from 2019-10-28
         */
        enum class ContextType : unsigned char
        {
            ipv4   = 1,
            ipv6   = 2, ///< note, 2 was for ipv4v6 in "TCP(IP) AT Commands Manual v1.0"
            ipv4v6 = 3
        };

        /**
         * @brief APN Authorization method
         */
        enum class AuthMethod : unsigned char
        {
            NONE = 0,
            PAP  = 1,
            CHAP = 2,
            AUTO = 3,
        };

        /**
         * @brief To configure whether to save <username> and <password> over CDMA network.
         */
        enum class SaveCDMAPwd : unsigned char
        {
            Disable = 0,
            Enable  = 1
        };

        enum class ContextState : unsigned char
        {
            Deactivated = 0,
            Activated   = 1
        };

        /**
         * @brief APN configuration data
         */
        class Config
        {
          public:
            unsigned char contextId =
                0; /// context on which apn is configured available values 1-16, 0 - means not set yet
            APNType apnType         = APNType::Internal;
            ContextState state      = ContextState::Deactivated;
            ContextType contextType = ContextType::ipv4; /// IP type
            AuthMethod authMethod   = AuthMethod::NONE;
            std::string apn; /// name of the APN
            std::string username;
            std::string password;
            std::string ip; /// set after connection

            std::string getAuthMethod()
            {
                return utils::enumToString(authMethod);
            }

            void setAuthMethod(const std::string &str)
            {
                if (str == "NONE")
                    authMethod = AuthMethod::NONE;
                else if (str == "AUTO")
                    authMethod = AuthMethod::AUTO;
                else if (str == "CHAP")
                    authMethod = AuthMethod::CHAP;
                else if (str == "PAP")
                    authMethod = AuthMethod::PAP;
                else
                    authMethod = AuthMethod::NONE;
            }

            std::string getApnType()
            {
                return utils::enumToString(apnType);
            }

            void setApnType(const std::string &str)
            {
                if (str == "Default")
                    apnType = APNType::Default;
                else if (str == "Fota")
                    apnType = APNType::Fota;
                else if (str == "IMS")
                    apnType = APNType::IMS;
                else if (str == "Internet")
                    apnType = APNType::Internet;
                else if (str == "MMS")
                    apnType = APNType::MMS;
                else
                    apnType = APNType::Internet;
            }

            std::string getApnProtocol()
            {
                return utils::enumToString(contextType);
            }

            void setApnProtocol(const std::string &str)
            {
                if (str == "ipv4")
                    contextType = ContextType::ipv4;
                else if (str == "ipv6")
                    contextType = ContextType::ipv6;
                else if (str == "ipv4v6")
                    contextType = ContextType::ipv4v6;
                else
                    contextType = ContextType::ipv4;
            }

            bool isEmpty() const noexcept
            {
                return apn.empty();
            }

            bool compare(std::shared_ptr<Config> c2)
            {
                return (this->apn == c2->apn) && (this->contextId == c2->contextId) &&
                       (this->contextType == c2->contextType) && (this->password == c2->password) &&
                       (this->username == c2->username) && (this->authMethod == c2->authMethod);
            }
            /**
             * @brief Compare two APNs only by elements essential for modem
             */
            friend bool operator==(const Config &c1, const Config &c2)
            {
                return (c1.apn == c2.apn) && (c1.contextId == c2.contextId) && (c1.contextType == c2.contextType) &&
                       (c1.password == c2.password) && (c1.username == c2.username) && (c1.authMethod == c2.authMethod);
            }

            friend bool operator!=(const Config &c1, const Config &c2)
            {
                return !(c1 == c2);
            }

            void from_json(json11::Json json)
            {
                contextId = json[json::APN::Config::contextId].int_value();
                apnType   = static_cast<packet_data::APN::APNType>(json[json::APN::Config::apnType].int_value());
                contextType =
                    static_cast<packet_data::APN::ContextType>(json[json::APN::Config::contextType].int_value());
                authMethod = static_cast<packet_data::APN::AuthMethod>(json[json::APN::Config::authMethod].int_value());
                apn        = json[json::APN::Config::apn].string_value();
                username   = json[json::APN::Config::username].string_value();
                password   = json[json::APN::Config::password].string_value();
            }
            [[nodiscard]] auto to_json() const -> json11::Json
            {
                return json11::Json::object{{json::APN::Config::contextId, contextId},
                                            {json::APN::Config::apnType, static_cast<int>(apnType)},
                                            {json::APN::Config::contextType, static_cast<int>(contextType)},
                                            {json::APN::Config::authMethod, static_cast<int>(authMethod)},
                                            {json::APN::Config::apn, apn},
                                            {json::APN::Config::username, username},
                                            {json::APN::Config::password, password}};
            }
        };

    } // namespace APN
    constexpr unsigned char EmptyContextId = 0;
    constexpr unsigned char MINContextId   = 1;
    constexpr unsigned char MAXContextId   = 16;

    /** Comment from quectel (2020.12):
     * As I know, only VZW MBN would use IMS on CID 1 to register IMS service directly.
     * So we usually ask customers to set up data connection on CID3 for VZW sim card.
     * Regarding other MBN files, the CID 1 shouldn’t be IMS because the CID1 is used
     * to activate default bearer for LTE network. So we usually ask customers to
     * configure their own APN on CID1.  With this rule, it’s easy for customer
     * to configure their APN no matter which MBN file is activated
     */
    constexpr auto internalAPNMaxId = 4;
    using ContextMap                = std::unordered_map<unsigned char, std::shared_ptr<APN::Config>>;
    using ContextPair               = std::pair<unsigned char, std::shared_ptr<APN::Config>>;

} // namespace packet_data
