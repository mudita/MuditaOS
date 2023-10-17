// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"
#include "SimState.hpp"

namespace at::urc
{

    /// +CPIN: <state> - Indicate SIM card pin state
    class Cpin : public Urc
    {
        enum Tokens
        {
            State = 0
        };

      public:
        static constexpr auto cpin_ready     = "READY";     ///<  MT is not pending for any password
        static constexpr auto cpin_not_ready = "NOT READY"; ///< +CPIN urc, SIM not ready
        static constexpr auto cpin_sim_pin   = "SIM PIN";   ///< MT is waiting for SIM PIN to be given
        static constexpr auto cpin_sim_puk   = "SIM PUK";   ///< MT is waiting for SIM PUK to be given
        static constexpr auto cpin_sim_pin2  = "SIM PIN2";  ///< MT is waiting for SIM PIN2 to be given
        static constexpr auto cpin_sim_puk2  = "SIM PUK2";  ///<  MT is waiting for SIM PUK2 to be given
        static constexpr auto cpin_phnet_pin =
            "PH-NET PIN"; ///<  MT is waiting for network personalization password to be given
        static constexpr auto cpin_phnet_puk =
            "PH-NET PUK"; ///<  MT is waiting for network personalization unblocking password
        static constexpr auto cpin_phnestsub_pin =
            "PH-NETSUB PIN"; ///<  MT is waiting for network subset personalization password to be given
        static constexpr auto cpin_phnestsub_puk =
            "PH-NETSUB PUK"; ///<  MT is waiting for network subset personalization unblocking password to be given
        static constexpr auto cpin_phsp_pin =
            "PH-SP PIN"; ///<  MT is waiting for service provider personalization password to be given
        static constexpr auto cpin_phsp_puk =
            "PH-SP PUK"; ///<  MT is waiting for service provider personalization unblocking password to be given
        static constexpr auto cpin_phcorp_pin =
            "PH-CORP PIN"; ///<  MT is waiting for corporate personalization password to be given
        static constexpr auto cpin_phcorp_puk =
            "PH-CORP PUK"; ///<  MT is waiting for corporate personalization unblocking password to be given

        static constexpr auto head = "+CPIN";

        static bool isURC(const std::string &uHead)
        {
            return uHead.find(Cpin::head) != std::string::npos;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;
        [[nodiscard]] auto getMessage() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getState() const noexcept -> std::optional<at::SimState>;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }

        static at::SimState parseState(std::string_view state);
    };

} // namespace at::urc
