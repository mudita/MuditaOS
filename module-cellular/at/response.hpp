// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <log/log.hpp>
#include <map>
#include <memory>
#include <string>

#include <service-appmgr/service-appmgr/data/MmiActionsParams.hpp>

namespace at
{
    namespace response
    {
        constexpr auto StringDelimiter = "\"";
        namespace qpinc
        {
            /// Structure that holds parsed information from AT+QPINC command
            struct AttemptsCounters
            {
                int PinCounter; /*!<  PIN attempts counter */
                int PukCounter; /*!<  PUK attempts counter */
            };
        } // namespace qpinc

        namespace cops
        {
            enum class OperatorStatus
            {
                Unknown   = 0,
                Available = 1,
                Current   = 2,
                Forbidden = 3
            };
            enum class CopsMode
            {
                Automatic    = 0,
                Manual       = 1,
                Deregister   = 2,
                OnlyFormat   = 3,
                ManualOrAuto = 4
            };
            enum class AccessTechnology
            {

                GSM                     = 0,
                UTRAN                   = 2,
                GSM_W_EGPRS             = 3,
                UTRAN_W_HSDPA           = 4,
                UTRAN_W_HSUPA           = 5,
                UTRAN_W_HSDPA_and_HSUPA = 6,
                E_UTRAN                 = 7,
                CDMA                    = 100
            };
            enum class NameFormat
            {
                Long    = 0,
                Short   = 1,
                Numeric = 2
            };

            class Operator
            {
              public:
                OperatorStatus status;
                std::string shortName;
                std::string longName;
                std::string numericName;
                std::optional<cops::AccessTechnology> technology = std::nullopt;

                std::string getNameByFormat(NameFormat format)
                {
                    switch (format) {
                    case NameFormat::Long:
                        return longName;
                    case NameFormat::Short:
                        return shortName;
                    case NameFormat::Numeric:
                        return numericName;
                    }
                    return {};
                }

                void setNameByFormat(NameFormat format, const std::string &name)
                {
                    switch (format) {
                    case NameFormat::Long:
                        longName = name;
                        break;
                    case NameFormat::Short:
                        shortName = name;
                        break;
                    case NameFormat::Numeric:
                        numericName = name;
                        break;
                    }
                }
            };

            class CurrentOperatorInfo
            {
                Operator op;
                CopsMode mode     = CopsMode::Automatic;
                NameFormat format = NameFormat::Long;
                bool operatorSet  = false;

              public:
                void setFormat(NameFormat format)
                {
                    this->format = format;
                }
                NameFormat getFormat() const noexcept
                {
                    return this->format;
                }
                void setMode(CopsMode mode)
                {
                    this->mode = mode;
                }
                CopsMode getMode() const noexcept
                {
                    return this->mode;
                }

                void setOperator(Operator op)
                {
                    this->operatorSet = true;
                    this->op          = op;
                }
                std::optional<cops::Operator> getOperator() const
                {
                    if (operatorSet) {
                        return op;
                    }
                    else {
                        return std::nullopt;
                    }
                }
            };
        } // namespace cops

        /**
         * @brief parse for AT+COPS=? from quectel
         *
         */
        bool parseCOPS(const at::Result &resp, std::vector<cops::Operator> &ret);
        using ResponseTokens = std::vector<std::vector<std::string>>;
        /**
         * @brief parse for AT+COPS? from quectel
         *
         */
        bool parseCOPS(const at::Result &resp, cops::CurrentOperatorInfo &ret);

        std::vector<std::string> tokenize(std::string &response, std::string separator = ",");

        /**
         * For AT one line (+XYZ) response like:
         * +CPIN READY
         * OK
         */
        std::optional<std::vector<std::string>> getTokensForATCommand(const at::Result &resp, std::string_view head);

        /**
         * For AT multiline response like (last OK)
         * +QIACT:1,<context_state>,<context_type>[,<IP_address>]
         * [.....
         * +QIACT:16,<context_state>,<context_type>[,<IP_address>]]
         * OK
         *
         * response from function like QPING (not mention in DOC as URC), looks like (first OK)
         * OK
         * +QPING: 0,"61.135.169.125",32,192,255
         * +QPING: 0,"61.135.169.125",32,240,255
         * ...
         * +QPING: 0,4,4,0,192,479,287
         *
         * Warning: should not be used for URC !
         */
        std::optional<ResponseTokens> getTokensForATResults(const at::Result &resp, std::string_view head);
        bool parseCSQ(std::string response, std::string &result);
        bool parseCSQ(std::string cellularResponse, uint32_t &result);
        bool parseCREG(std::string &response, uint32_t &result);
        bool parseCREG(std::string &response, std::string &result);
        bool parseQNWINFO(const std::string &response, std::string &result);
        bool parseQPINC(const at::Result &resp, qpinc::AttemptsCounters &ret);
        bool parseCLCK(const at::Result &resp, int &ret);

        namespace creg
        {
            bool isRegistered(uint32_t commandData);
        }
        namespace qnwinfo
        {

            auto constexpr gsmString   = "GSM";
            auto constexpr wcdmaString = "WCDMA";

            auto constexpr band_1  = 1;
            auto constexpr band_2  = 2;
            auto constexpr band_3  = 3;
            auto constexpr band_4  = 4;
            auto constexpr band_5  = 5;
            auto constexpr band_7  = 7;
            auto constexpr band_8  = 8;
            auto constexpr band_12 = 12;
            auto constexpr band_13 = 13;
            auto constexpr band_18 = 18;
            auto constexpr band_19 = 19;
            auto constexpr band_20 = 20;
            auto constexpr band_25 = 25;
            auto constexpr band_26 = 26;
            auto constexpr band_28 = 28;
            auto constexpr band_38 = 38;
            auto constexpr band_40 = 40;
            auto constexpr band_41 = 41;

            auto constexpr band_3_freq = 1800;
            auto constexpr band_1_freq = 2100;
            auto constexpr band_2_freq = 1900;

            auto constexpr band_4_freq  = 1700;
            auto constexpr band_5_freq  = 850;
            auto constexpr band_7_freq  = 2600;
            auto constexpr band_8_freq  = 900;
            auto constexpr band_12_freq = 700;
            auto constexpr band_13_freq = 700;
            auto constexpr band_18_freq = 850;
            auto constexpr band_19_freq = 850;
            auto constexpr band_20_freq = 800;
            auto constexpr band_25_freq = 1900;
            auto constexpr band_26_freq = 850;
            auto constexpr band_28_freq = 700;
            auto constexpr band_38_freq = 2600;
            auto constexpr band_40_freq = 2300;
            auto constexpr band_41_freq = 2500;

            uint32_t parseNetworkFrequency(std::string &response);
            uint32_t parseNumericBandString(std::string &string);
            uint32_t parseLteBandString(std::string &string);
        } // namespace qnwinfo

        namespace clir
        {
            constexpr uint32_t clirTokens = 2;

            enum class ServiceState
            {
                AccordingToSubscription,
                ServiceEnabled,
                ServiceDisabled,
            };

            enum class ServiceStatus
            {
                NotProvisioned,
                PermanentProvisioned,
                Unknown,
                TemporaryRestricted,
                TemporaryAllowed
            };

            class ClirResponse
            {
              public:
                ServiceState serviceState;
                ServiceStatus serviceStatus;

                explicit ClirResponse(const ServiceState &state, const ServiceStatus &status)
                    : serviceState(state), serviceStatus(status)
                {}
            };

            auto parseClir(const std::string &response) -> std::optional<ClirResponse>;
            auto getState(const ServiceState &state) -> app::manager::actions::IMMICustomResultParams::MMIResultMessage;
            auto getStatus(const ServiceStatus &status)
                -> app::manager::actions::IMMICustomResultParams::MMIResultMessage;
        } // namespace clir
    }     // namespace response
} // namespace at
