// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UrcCpin.hpp"
#include <map>

using namespace at::urc;

at::SimState Cpin::parseState(std::string_view state)
{
    const std::map<std::string_view, SimState> cpin_convert_map = {
        {Cpin::cpin_ready, SimState::Ready},
        {Cpin::cpin_not_ready, SimState::NotReady},
        {Cpin::cpin_sim_pin, SimState::SimPin},
        {Cpin::cpin_sim_puk, SimState::SimPuk},
        {Cpin::cpin_sim_pin2, SimState::SimPin2},
        {Cpin::cpin_sim_puk2, SimState::SimPuk2},
        {Cpin::cpin_phnet_pin, SimState::PhNetPin},
        {Cpin::cpin_phnet_puk, SimState::PhNetPuk},
        {Cpin::cpin_phnestsub_pin, SimState::PhNetSPin},
        {Cpin::cpin_phnestsub_puk, SimState::PhNetSPuk},
        {Cpin::cpin_phsp_pin, SimState::PhSpPin},
        {Cpin::cpin_phsp_puk, SimState::PhSpPuk},
        {Cpin::cpin_phcorp_pin, SimState::PhCorpPin},
        {Cpin::cpin_phcorp_puk, SimState::PhCorpPuk},

    };

    auto it = cpin_convert_map.find(state);

    if (it != cpin_convert_map.end()) {
        return it->second;
    }
    return SimState::Unknown;
}

auto Cpin::isValid() const noexcept -> bool
{
    return tokens.size() == magic_enum::enum_count<Tokens>();
}

auto Cpin::getMessage() const noexcept -> std::optional<std::string>
{
    if (!isValid()) {
        return std::nullopt;
    }
    return tokens[Tokens::State];
}

auto Cpin::getState() const noexcept -> std::optional<at::SimState>
{
    if (!isValid()) {
        return std::nullopt;
    }
    auto msg = getMessage();
    LOG_DEBUG("CPIN State: %s", (*msg).c_str());
    return parseState(*msg);
}
