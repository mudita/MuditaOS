// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URC_QIND.hpp>
#include <log/debug.hpp>

using namespace at::urc;

QIND::QIND(const std::string &val) : Any(val)
{}

auto QIND::what() const noexcept -> std::string
{
    return urc_name;
}

auto QIND::isCsq() const noexcept -> bool
{
    if (tokens.size() == magic_enum::enum_count<enum CSQ>()) {
        return tokens[CSQ].find(type_csq) != std::string::npos;
    }
    return false;
}

auto QIND::validate(enum CSQ check) const noexcept -> bool
{
    try {
        if (isCsq()) {
            int rssi = std::stoi(tokens[RSSI]);
            int ber  = std::stoi(tokens[BER]);
            LOG_DEBUG("> %d %d", rssi, ber);
            switch (check) {
            case RSSI:
                return rssi != invalid_rssi_low && rssi != invalid_rssi_high;
            case BER:
                return ber != invalid_ber;
            default:
                return false;
            }
        }
    }
    catch (const std::exception &ex) {
        LOG_FATAL("exception: %s", ex.what());
    }
    return false;
}

auto QIND::getRSSI() const noexcept -> std::optional<int>
{
    if (isCsq()) {
        int rssi;
        try {
            rssi = std::stoi(tokens[RSSI]);
        }
        catch (const std::exception &e) {
            return std::nullopt;
        }

        if (rssi != invalid_rssi_low && rssi != invalid_rssi_high) {
            return rssi;
        }
    }

    return std::nullopt;
}

auto QIND::getBER() const noexcept -> std::optional<int>
{
    if (isCsq()) {
        int ber;
        try {
            ber = std::stoi(tokens[BER]);
        }
        catch (const std::exception &e) {
            return std::nullopt;
        }

        if (ber != invalid_ber) {
            return ber;
        }
    }

    return std::nullopt;
}
