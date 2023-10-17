// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>
#include <PhoneNumber.hpp>

namespace ModemCall
{
    enum class CallState : uint8_t
    {
        Active = 0, // 0 active: call in progress (setup was successful)
        Held,       // 1 held: call on hold
        Dialing,    // 2 dialing (MO call): number dialed
        Alerting,   // 3 alerting (MO call): number dialed and the called party is alerted
        Incoming,   // 4 incoming (MT call): incoming call, ringtone played (AT RING notification)
        Waiting // 5 waiting (MT call): call waiting notification while another call is active (if call waiting feature
                // enabled)
    };

    enum class CallDir : uint8_t
    {
        MO = 0, // Mobile originated (MO) call
        MT = 1  // Mobile terminated (MT) call
    };

    enum class CallMode : uint8_t
    {
        Voice = 0,
        Data,
        FAX
    };
} // namespace ModemCall

namespace at
{
    namespace cmd
    {
        class CLCC;
    } // namespace cmd

    namespace result
    {
        /// please see documentation:
        /// QuectelEC2526EC21ATCommandsManualV13.1100970659
        /// page: 101 for more information
        struct CLCC : public Result
        {
          private:
            struct Data
            {
                const std::uint8_t idx;
                const ModemCall::CallDir dir;
                const ModemCall::CallState stateOfCall;
                const ModemCall::CallMode mode;
                const bool multiparty;
                const utils::PhoneNumber::View number;
                const std::string type;
                const std::string alpha;
                const std::size_t tokens;
            };

            std::vector<Data> data;
            friend cmd::CLCC;

          public:
            explicit CLCC(const Result &);
            [[nodiscard]] auto getData() const noexcept -> const std::vector<Data> &
            {
                return data;
            };
        };
    } // namespace result

    namespace cmd
    {
        class CLCC : public Cmd
        {
          protected:
            [[nodiscard]] static auto toBool(const std::string &text) -> bool;
            [[nodiscard]] static auto toUInt(const std::string &text) -> std::uint8_t;
            template <typename T>
            [[nodiscard]] static auto toEnum(const std::string &text) -> std::optional<T>;

          public:
            CLCC() noexcept;
            explicit CLCC(at::cmd::Modifier mod) noexcept;
            [[nodiscard]] auto parseCLCC(const Result &base_result) -> result::CLCC;
        };
    } // namespace cmd
} // namespace at
