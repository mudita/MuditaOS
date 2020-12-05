// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <memory>

namespace packet_data
{

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
            Default, ///< for data traffic
            IMS,     ///< IP Multimedia Subsystem for eg VoLTE
            MMS,     ///< for MMS service
            Fota     ///< for Firmware Update
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
            APNType apnType         = APNType::Default;
            ContextState state      = ContextState::Deactivated;
            ContextType contextType = ContextType::ipv4; /// IP type
            AuthMethod authMethod   = AuthMethod::NONE;
            std::string apn; /// name of the APN
            std::string username;
            std::string password;
            std::string ip; /// set after connection

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
        };

    } // namespace APN

    constexpr unsigned char MINContextId = 1;
    constexpr unsigned char MAXContextId = 16;
    using ContextMap                     = std::unordered_map<unsigned char, std::shared_ptr<APN::Config>>;
    using ContextPair                    = std::pair<unsigned char, std::shared_ptr<APN::Config>>;

} // namespace packet_data
